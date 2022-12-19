import numpy as np
from matplotlib import pyplot as plt

from src.tool.dataset import initNonSNRStack, initLoRaWAN, RealEDNum, loadEDjFinalSF, loadEDjFinalTP


def drawAssignedSF():
    # Choose font
    plt.rc('font', family='Times New Roman')
    # x为每组柱子x轴的基准位置
    labels = ['1', '2', '3', '4']

    # Init datasets
    NonSNRStackDataset = initNonSNRStack()
    DyLoRaDataset = initLoRaWAN()

    # Load datasets
    y1 = []
    y2 = []

    datas = [y1, y2]  # http://t.csdn.cn/53Uvl

    for loopcount in range(RealEDNum):
        y1.append(loadEDjFinalSF(loopcount, NonSNRStackDataset[9]))
        y2.append(loadEDjFinalSF(loopcount, DyLoRaDataset[9]))

    print(y1)
    # Initialize subplot
    fig, ax1 = plt.subplots()

    tick_step = 1

    group_gap = 0.6
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
            plt.bar(x + index * bar_span, y, bar_width, label='EELoRa')
        elif index == 1:
            plt.bar(x + index * bar_span, y, bar_width, label='LoRaWAN')

    ax1.set_ylabel('Spreading Factor', fontsize=15)

    # ticks为新x轴刻度标签位置，即每组柱子x轴上的中心位置
    ticks = x + (group_width - bar_span) / 2
    plt.xticks(ticks, labels)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw legends
    plt.legend(loc='best',
               fontsize=14,
               ncol=3)

    # Draw gridlines
    ax1.grid()

    plt.savefig("bin/SF(Parameter).pdf", format="pdf", transparent="ture", dpi=300, bbox_inches='tight')

    plt.show()

    return

def drawAssignedTP():
    # Choose font
    plt.rc('font', family='Times New Roman')
    # x为每组柱子x轴的基准位置
    labels = ['1', '2', '3', '4']

    # Init datasets
    NonSNRStackDataset = initNonSNRStack()
    DyLoRaDataset = initLoRaWAN()

    # Load datasets
    y1 = []
    y2 = []

    datas = [y1, y2]  # http://t.csdn.cn/53Uvl

    for loopcount in range(RealEDNum):
        y1.append(loadEDjFinalTP(loopcount, NonSNRStackDataset[9]))
        y2.append(loadEDjFinalTP(loopcount, DyLoRaDataset[9]))

    # Initialize subplot
    fig, ax1 = plt.subplots()

    tick_step = 1

    group_gap = 0.6
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
            plt.bar(x + index * bar_span, y, bar_width, label='EELoRa')
        elif index == 1:
            plt.bar(x + index * bar_span, y, bar_width, label='LoRaWAN')

    ax1.set_ylabel('Transmission Power', fontsize=15)

    # ticks为新x轴刻度标签位置，即每组柱子x轴上的中心位置
    ticks = x + (group_width - bar_span) / 2
    plt.xticks(ticks, labels)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw legends
    plt.legend(loc='best',
               fontsize=14,
               ncol=3)

    # Draw gridlines
    ax1.grid()

    plt.savefig("bin/TP(Parameter).pdf", format="pdf", transparent="ture", dpi=300, bbox_inches='tight')

    plt.show()

    return