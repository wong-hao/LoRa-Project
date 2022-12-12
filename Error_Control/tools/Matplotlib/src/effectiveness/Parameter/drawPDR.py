import numpy as np
from matplotlib import pyplot as plt

# http://t.csdn.cn/jxmCD
# https://www.jb51.net/article/206226.htm

# Draw a parallel histogram
from matplotlib.ticker import FuncFormatter

from src.tool.Avg import getAvg
from src.tool.Dataset import loadNSThroughput, loadCSThroughput, initCSPara, loadCSPDR

TX_INTERVAL = 10
pendTxLen = 28

TPLevelNum = 3
SFNum = 6

# http://t.csdn.cn/jxmCD
# https://www.jb51.net/article/206226.htm

def drawEffPDR():
    # Choose font
    plt.rc('font', family='Times New Roman')
    # x为每组柱子x轴的基准位置
    labels = ['SF=7', 'SF=8', 'SF=9', 'SF=10', 'SF=11', 'SF=12']

    # Init datasets
    CSParaDataset = initCSPara()

    TP0 = []
    TP1 = []
    TP2 = []

    # 3 TP Levels: low, mid, and high
    for loopcount in range(SFNum * TPLevelNum):
        (averagePDR, averagePDRPoints) = getAvg(loadCSPDR(CSParaDataset[loopcount]))

        if loopcount % TPLevelNum == 0:
            TP0.append(averagePDR)
        elif loopcount % TPLevelNum == 1:
            TP1.append(averagePDR)
        elif loopcount % TPLevelNum == 2:
            TP2.append(averagePDR)

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

    ax1.set_ylabel('Packet Delivery Ratio', fontsize=15)

    # ticks为新x轴刻度标签位置，即每组柱子x轴上的中心位置
    ticks = x + (group_width - bar_span) / 2
    plt.xticks(ticks, labels)

    # Initialize yxis
    ax1.set_ylim(0, 1)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw legends
    plt.legend(loc='best',
               fontsize=14,
               ncol=3)

    # Draw gridlines
    ax1.grid()

    plt.savefig("bin/PDR(Parameter).pdf", format="pdf", transparent="ture", dpi=300, bbox_inches='tight')

    plt.show()

    return
