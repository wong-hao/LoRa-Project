import numpy as np
from matplotlib import pyplot as plt

from src.tool.dataset import initNonSNRStack, initLoRaWAN, RealEDNum, loadEDjFinalSF, loadEDjFinalTP, initDyLoRa, \
    MidDataset, TotalDataset, TxpowerArrayWatt, initEFLoRa


def drawAssignedSF():
    # Choose font
    plt.rc('font', family='Times New Roman')
    # x为每组柱子x轴的基准位置
    labels = ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12']

    # Initialize datasets
    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()
    DyLoRaDataset = initDyLoRa()
    LoRaWANDataset = initLoRaWAN()

    # Load datasets
    y1 = []
    y2 = []
    y3 = []
    y4 = []

    datas = [y1, y2, y3, y4]  # http://t.csdn.cn/53Uvl

    for loopcount in range(TotalDataset):
        if loopcount == TotalDataset - 1:
            # 求最后一一行所有节点的SF
            for loopcount2 in range(RealEDNum):
                y1.append(loadEDjFinalSF(loopcount2, NonSNRStackDataset[loopcount]))
                y2.append(loadEDjFinalSF(loopcount2, EFLoRaDataset[loopcount]))
                y3.append(loadEDjFinalSF(loopcount2, DyLoRaDataset[loopcount]))
                y4.append(loadEDjFinalSF(loopcount2, LoRaWANDataset[loopcount]))

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
    # 绘制柱子
    for index, y in enumerate(datas):
        if index == 0:
            plt.bar(x + index * bar_span, y, bar_width, label='EEADR')
        elif index == 1:
            plt.bar(x + index * bar_span, y, bar_width, label='EFLoRa')
        elif index == 2:
            plt.bar(x + index * bar_span, y, bar_width, label='DyLoRa')
        # elif index == 3:
            # plt.bar(x + index * bar_span, y, bar_width, label='NS-side ADR')

    ax1.set_ylabel('Spreading Factor', fontsize=15)

    # ticks为新x轴刻度标签位置，即每组柱子x轴上的中心位置
    ticks = x + (group_width - bar_span) / 2
    plt.xticks(ticks, labels)

    # Initialize yxis
    ax1.set_ylim(0, 13.5)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw legends
    plt.legend(loc='best',
               fontsize=10,
               ncol=4)

    # Draw gridlines
    ax1.grid()

    plt.savefig("bin/SF(Parameter).svg", format="svg", transparent="ture", dpi=300, bbox_inches='tight')

    plt.show()

    return

def drawAssignedTP():
    # Choose font
    plt.rc('font', family='Times New Roman')
    # x为每组柱子x轴的基准位置
    labels = ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12']

    # Initialize datasets
    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()
    DyLoRaDataset = initDyLoRa()
    LoRaWANDataset = initLoRaWAN()

    # Load datasets
    y1 = []
    y2 = []
    y3 = []
    y4 = []

    datas = [y1, y2, y3, y4]  # http://t.csdn.cn/53Uvl

    for loopcount in range(TotalDataset):
        if loopcount == TotalDataset-1:
            # 求最后一一行所有节点的TP
            for loopcount2 in range(RealEDNum):
                y1index = loadEDjFinalTP(loopcount2, NonSNRStackDataset[loopcount])
                y1.append(TxpowerArrayWatt[int(y1index)])
                y2index = loadEDjFinalTP(loopcount2, EFLoRaDataset[loopcount])
                y2.append(TxpowerArrayWatt[int(y2index)])
                y3index = loadEDjFinalTP(loopcount2, DyLoRaDataset[loopcount])
                y3.append(TxpowerArrayWatt[int(y3index)])
                y4index = loadEDjFinalTP(loopcount2, LoRaWANDataset[loopcount])
                y4.append(TxpowerArrayWatt[int(y4index)])

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
            plt.bar(x + index * bar_span, y, bar_width, label='EEADR')
        elif index == 1:
            plt.bar(x + index * bar_span, y, bar_width, label='EFLoRa')
        elif index == 2:
            plt.bar(x + index * bar_span, y, bar_width, label='DyLoRa')
        # elif index == 3:
            # plt.bar(x + index * bar_span, y, bar_width, label='NS-side ADR')

    ax1.set_ylabel('Transmission Power (mW)', fontsize=15)

    # ticks为新x轴刻度标签位置，即每组柱子x轴上的中心位置
    ticks = x + (group_width - bar_span) / 2
    plt.xticks(ticks, labels)

    # Initialize yxis
    ax1.set_ylim(0, 1100)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw legends
    plt.legend(loc='best',
               fontsize=10,
               ncol=4)

    # Draw gridlines
    ax1.grid()

    plt.savefig("bin/TP(Parameter).svg", format="svg", transparent="ture", dpi=300, bbox_inches='tight')

    plt.show()

    return