import numpy as np
from matplotlib import pyplot as plt

from src.tool import Dataset
from src.tool.Avg import getAvgNum
from src.tool.Dataset import initCSPara, loadCSThroughput, SFNum, TPNum, TPLevelNum, loadCSFinalThroughput, n


# http://t.csdn.cn/jxmCD
# https://www.jb51.net/article/206226.htm

def drawEffThroughput():
    # Choose font
    plt.rc('font', family='Times New Roman')
    # x为每组柱子x轴的基准位置
    labels = ['SF=7', 'SF=8', 'SF=9', 'SF=10', 'SF=11', 'SF=12']

    # Initialize datasets
    CSParaDataset = initCSPara()

    # # 8 TP Levels
    # for loopcount in range(SFNum * TPNum):
    #
    #     # Load datasets
    #
    #     if loopcount % TPNum == 0:
    #         TP0.append(getAvgNum(loadCSThroughput(CSParaDataset[loopcount])))
    #     elif loopcount % TPNum == 1:
    #         TP1.append(getAvgNum(loadCSThroughput(CSParaDataset[loopcount])))
    #     elif loopcount % TPNum == 2:
    #         TP2.append(getAvgNum(loadCSThroughput(CSParaDataset[loopcount])))
    #     elif loopcount % TPNum == 3:
    #         TP3.append(getAvgNum(loadCSThroughput(CSParaDataset[loopcount])))
    #     elif loopcount % TPNum == 4:
    #         TP4.append(getAvgNum(loadCSThroughput(CSParaDataset[loopcount])))
    #     elif loopcount % TPNum == 5:
    #         TP5.append(getAvgNum(loadCSThroughput(CSParaDataset[loopcount])))
    #     elif loopcount % TPNum == 6:
    #         TP6.append(getAvgNum(loadCSThroughput(CSParaDataset[loopcount])))
    #     else:
    #         TP7.append(getAvgNum(loadCSThroughput(CSParaDataset[loopcount])))
    #
    #     datas = [TP0, TP1, TP2, TP3, TP4, TP5, TP6, TP7]  # http://t.csdn.cn/53Uvl

    TP0data0 = []
    TP0data1 = []
    TP0data2 = []
    TP1data0 = []
    TP1data1 = []
    TP1data2 = []
    TP2data0 = []
    TP2data1 = []
    TP2data2 = []

    # 3 TP Levels: low, mid, and high
    for loopcount in range(n * SFNum * TPLevelNum):

        if loopcount % (n * TPLevelNum) == 0:
            TP0data0.append(loadCSFinalThroughput(CSParaDataset[loopcount]))
        elif loopcount % (n * TPLevelNum) == 1:
            TP0data1.append(loadCSFinalThroughput(CSParaDataset[loopcount]))
        elif loopcount % (n * TPLevelNum) == 2:
            TP0data2.append(loadCSFinalThroughput(CSParaDataset[loopcount]))
        elif loopcount % (n * TPLevelNum) == 3:
            TP1data0.append(loadCSFinalThroughput(CSParaDataset[loopcount]))
        elif loopcount % (n * TPLevelNum) == 4:
            TP1data1.append(loadCSFinalThroughput(CSParaDataset[loopcount]))
        elif loopcount % (n * TPLevelNum) == 5:
            TP1data2.append(loadCSFinalThroughput(CSParaDataset[loopcount]))
        elif loopcount % (n * TPLevelNum) == 6:
            TP2data0.append(loadCSFinalThroughput(CSParaDataset[loopcount]))
        elif loopcount % (n * TPLevelNum) == 7:
            TP2data1.append(loadCSFinalThroughput(CSParaDataset[loopcount]))
        elif loopcount % (n * TPLevelNum) == 8:
            TP2data2.append(loadCSFinalThroughput(CSParaDataset[loopcount]))

    ZippedTP0 = zip(TP0data0, TP0data1, TP0data2)
    ZippedTP1 = zip(TP1data0, TP1data1, TP1data2)
    ZippedTP2 = zip(TP2data0, TP2data1, TP2data2)

    TP0 = [(a + b + c) / n for a, b, c in ZippedTP0]
    TP1 = [(a + b + c) / n for a, b, c in ZippedTP1]
    TP2 = [(a + b + c) / n for a, b, c in ZippedTP2]

    datas = [TP0, TP1, TP2]  # http://t.csdn.cn/53Uvl

    # Initialize subplots
    fig, ax1 = plt.subplots()

    tick_step = 1

    group_gap = 0.2
    bar_gap = 0

    x = np.arange(len(labels)) * tick_step
    # group_num为数据的组数，即每组柱子的柱子个数
    group_num = len(datas)
    # group_width为每组柱子的总宽度，group_gap 为柱子组与组之间的间隙。
    group_width = tick_step - group_gap
    # bar_span为每组柱子之间在x轴上的距离，即柱子宽度和间隙的总和
    bar_span = group_width / group_num
    # bar_width为每个柱子的实际宽度
    bar_width = bar_span - bar_gap

    # Overlay Histogram
    # ax1.bar(x + 1 * bar_span, TP0, bar_width, label='TP=Low')
    # ax1.bar(x + 1 * bar_span, TP1, bar_width, bottom=TP0, label='TP=Mid')
    #
    # bottom = []  # http://t.csdn.cn/rujaL
    # for i in range(0, len(TP0)):
    #     summm = TP0[i] + TP1[i]
    #     bottom.append(summm)
    #
    # ax1.bar(x + 1 * bar_span, TP2, bar_width, bottom=bottom, label='TP=High')

    # 绘制柱子
    for index, y in enumerate(datas):
        if index == 0:
            plt.bar(x + index * bar_span, y, bar_width, label='TP=' + 'Low')
        elif index == 1:
            plt.bar(x + index * bar_span, y, bar_width, label='TP=' + 'Mid')
        elif index == 2:
            plt.bar(x + index * bar_span, y, bar_width, label='TP=' + 'High')

    ax1.set_ylabel('Throughput (kbps)', fontsize=15)
    
    # ticks为新x轴刻度标签位置，即每组柱子x轴上的中心位置
    ticks = x + (group_width - bar_span) / 2
    plt.xticks(ticks, labels)

    # Initialize yxis
    ax1.set_ylim((0, 0.0182))

    # Choose tick pramaters
    ax1.ticklabel_format(axis="y", style="sci", scilimits=(0, 0))
    ax1.tick_params(labelsize=15)
    #ax1.yaxis.set_major_formatter(mtick.FormatStrFormatter('%.2f'))

    # Draw legends
    plt.legend(loc='best',
               fontsize=14,
               ncol=3)

    # Draw gridlines
    ax1.grid()

    # Save subplots to files
    plt.savefig("bin/Throughput(Parameter).pdf", format="pdf", transparent="ture", dpi=300, bbox_inches='tight')

    # Show subplots
    plt.show()

    return
