import numpy as np
from matplotlib import pyplot as plt

# http://t.csdn.cn/jxmCD
# https://www.jb51.net/article/206226.htm

# Draw a parallel histogram
from matplotlib.ticker import FuncFormatter

from src.tool.Avg import getAvg
from src.tool.formatnum import formatnum, to_percent, formatnum2
from src.tool.Dataset import loadNSThroughput, loadCSThroughput, initCSJXNum, \
    initNSJXNum, initCSPara

TX_INTERVAL = 10
pendTxLen = 28

TPNum = 8
TPLevelNum = 3
SFNum = 6


def drawInstantThroughput():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize number formatter
    scientificformatter = FuncFormatter(formatnum)
    scientificformatter2 = FuncFormatter(formatnum2)

    # Theoretical upper limit of throughput
    # theory = pendTxLen * 8 / (TX_INTERVAL * 1000)

    # Init datasets
    CSJXNumDataset = initCSJXNum()
    NSJXNumDataset = initNSJXNum()

    # Load datasets
    (x1, y1) = loadCSThroughput(CSJXNumDataset[3])
    (x2, y2) = loadNSThroughput(NSJXNumDataset[3])
    (x3, y3) = loadCSThroughput(CSJXNumDataset[0])
    (x4, y4) = loadNSThroughput(NSJXNumDataset[0])

    # Calculate average throughput1
    (averagethroughput1, averagethroughputPoints1) = getAvg((x1, y1))

    # Calculate average throughput2
    (averagethroughput2, averagethroughputPoints2) = getAvg((x2, y2))

    # Calculate average throughput3
    (averagethroughput3, averagethroughputPoints3) = getAvg((x3, y3))

    # Calculate average throughput4
    (averagethroughput4, averagethroughputPoints4) = getAvg((x4, y4))

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Initialize axis
    # ax1.set_ylim((0, 1))
    # ax1.xaxis.set_major_formatter(scientificformatter)
    # ax1.yaxis.set_major_formatter(scientificformatter2)
    ax1.ticklabel_format(axis="both", style="sci", scilimits=(0, 0))
    ax1.set_xlabel(r'Time (ms)', fontsize=15)
    ax1.set_ylabel(r'Throughput (kbps)', fontsize=15)

    # Draw lines
    ax1.plot(x1, y1, color='r', label=r'Instant (CS: disabled)')
    ax1.plot(x2, y2, color='b', label=r'Instant (NS: disabled)')
    ax1.plot(x3, y3, color='g', label=r'Instant (CS: enabled)')
    ax1.plot(x4, y4, color='y', label=r'Instant (NS: enabled)')

    ax1.plot(x1, averagethroughputPoints1, color='r', linestyle="--", label=r'Average (CS: disabled)')
    ax1.plot(x2, averagethroughputPoints2, color='b', linestyle="--", label=r'Average (NS: disabled)')
    ax1.plot(x3, averagethroughputPoints3, color='g', linestyle="--", label=r'Average (CS: enabled)')
    ax1.plot(x4, averagethroughputPoints4, color='y', linestyle="--", label=r'Average (NS: enabled)')

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw legends
    plt.legend(loc='best',
               fontsize=7,
               ncol=2)

    # Draw gridlines
    ax1.grid()

    # Draw title
    # plt.title(r'Instant Throughput')

    # Save subplots to files
    plt.savefig("bin/Throughput.pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return


def drawEffThroughput():
    # Choose font
    plt.rc('font', family='Times New Roman')
    # x为每组柱子x轴的基准位置
    labels = ['SF=7', 'SF=8', 'SF=9', 'SF=10', 'SF=11', 'SF=12']

    # Init datasets
    CSParaDataset = initCSPara()

    TP0 = []
    TP1 = []
    TP2 = []
    TP3 = []
    TP4 = []
    TP5 = []
    TP6 = []
    TP7 = []

    if len(CSParaDataset) == SFNum * TPNum:  # 8 TP Levels

        # Load datasets and calculate average throughputs

        for loopcount in range(SFNum * TPNum):
            (averagethroughput, averagethroughputPoints) = getAvg(loadCSThroughput(CSParaDataset[loopcount]))

            if loopcount % TPNum == 0:
                TP0.append(averagethroughput)
            elif loopcount % TPNum == 1:
                TP1.append(averagethroughput)
            elif loopcount % TPNum == 2:
                TP2.append(averagethroughput)
            elif loopcount % TPNum == 3:
                TP3.append(averagethroughput)
            elif loopcount % TPNum == 4:
                TP4.append(averagethroughput)
            elif loopcount % TPNum == 5:
                TP5.append(averagethroughput)
            elif loopcount % TPNum == 6:
                TP6.append(averagethroughput)
            else:
                TP7.append(averagethroughput)

        datas = [TP0, TP1, TP2, TP3, TP4, TP5, TP6, TP7]  # http://t.csdn.cn/53Uvl

    else:  # 3 TP Levels: low, mid, and high

        for loopcount in range(SFNum * TPLevelNum):
            (averagethroughput, averagethroughputPoints) = getAvg(loadCSThroughput(CSParaDataset[loopcount]))

            if loopcount % TPLevelNum == 0:
                TP0.append(averagethroughput)
            elif loopcount % TPLevelNum == 1:
                TP1.append(averagethroughput)
            elif loopcount % TPLevelNum == 2:
                TP2.append(averagethroughput)

        datas = [TP0, TP1, TP2]  # http://t.csdn.cn/53Uvl

    # Initialize subplot
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
    # 绘制柱子
    for index, y in enumerate(datas):
        if index == 0:
            plt.bar(x + index * bar_span, y, bar_width, label='TP=' + 'Low')
        elif index == 1:
            plt.bar(x + index * bar_span, y, bar_width, label='TP=' + 'Mid')
        elif index == 2:
            plt.bar(x + index * bar_span, y, bar_width, label='TP=' + 'High')
        else:
            plt.bar(x + index * bar_span, y, bar_width, label='TP=' + str(index))

    ax1.set_ylabel('Throughput (kbps)', fontsize=15)

    # ticks为新x轴刻度标签位置，即每组柱子x轴上的中心位置
    ticks = x + (group_width - bar_span) / 2
    plt.xticks(ticks, labels)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw legends
    plt.legend()

    # Draw gridlines
    ax1.grid()

    plt.savefig("bin/ABC.pdf", format="pdf", transparent="ture", dpi=300, bbox_inches='tight')

    plt.show()

    return
