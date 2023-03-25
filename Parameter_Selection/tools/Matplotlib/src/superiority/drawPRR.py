import numpy as np
from matplotlib import pyplot as plt

from src.tool import dataset
from src.tool.dataset import initNonSNRStack, initLoRaWAN, loadEDjFinalAveragePRR, RealEDNum, initDyLoRa, MidDataset, \
    TotalDataset
from src.tool.formatfile import svg2emf


def drawPRR():
    # Choose font
    plt.rc('font', family='Times New Roman')
    # x为每组柱子x轴的基准位置
    labels = ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12']

    # Initialize datasets
    NonSNRStackDataset = initNonSNRStack()
    DyLoRaDataset = initDyLoRa()
    LoRaWANDataset = initLoRaWAN()

    # Load datasets
    y1 = []
    y2 = []
    y3 = []

    datas = [y1, y2, y3]  # http://t.csdn.cn/53Uvl

    for loopcount in range(TotalDataset):
        if loopcount == TotalDataset - 1:
            # 求最后一一行所有节点的PRR
            for loopcount2 in range(RealEDNum):
                y1.append(loadEDjFinalAveragePRR(loopcount2, NonSNRStackDataset[loopcount]))
                y2.append(loadEDjFinalAveragePRR(loopcount2, DyLoRaDataset[loopcount]))
                y3.append(loadEDjFinalAveragePRR(loopcount2, LoRaWANDataset[loopcount]))

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
            plt.bar(x + index * bar_span, y, bar_width, label='DyLoRa')
        elif index == 2:
            plt.bar(x + index * bar_span, y, bar_width, label='LoRaWAN')

    ax1.set_ylabel('Packet Reception Ratio', fontsize=15)

    # ticks为新x轴刻度标签位置，即每组柱子x轴上的中心位置
    ticks = x + (group_width - bar_span) / 2
    plt.xticks(ticks, labels)

    # Initialize yxis
    ax1.set_ylim(0, 1.15)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw legends
    plt.legend(loc='best',
               fontsize=13,
               ncol=3)

    # Draw gridlines
    ax1.grid()

    # Save subplots to files
    filename = "bin/PRR(Parameter).svg"
    plt.savefig(filename, format="svg", transparent="ture", dpi=300, bbox_inches='tight')
    svg2emf(filename)

    # Show subplots
    plt.show()

    return

