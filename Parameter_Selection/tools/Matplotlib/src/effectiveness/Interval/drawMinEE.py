import numpy as np
from matplotlib import pyplot as plt

from src.tool.dataset import initInterval, IntervalLevel, loadFinalMinEE
from src.tool.formatfile import svg2emf


def drawIntervalMinEE():
    # Choose font
    plt.rc('font', family='Times New Roman')
    # x为每组柱子x轴的基准位置
    labels = ['5', '10', '15', '20']

    # Initialize datasets
    Intervaldataset = initInterval()

    # Load datasets
    x = np.linspace(5, 20, 4)
    y = []

    for loopcount in range(IntervalLevel):

        minEE = loadFinalMinEE(Intervaldataset[loopcount])
        y.append(minEE)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    datas = [y]

    # Initialize subplot
    fig, ax1 = plt.subplots()

    tick_step = 1

    group_gap = 1.2
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

    # ticks为新x轴刻度标签位置，即每组柱子x轴上的中心位置
    ticks = x + (group_width - bar_span) / 2
    plt.xticks(ticks, labels)

    # Initialize axis
    # plt.xticks([1, 2, 3, 4], ['Tg=5', 'Tg=10', 'Interval=15', 'Interval=20'])
    ax1.set_xlabel(r'Packet transmission interval (s)', fontsize=15)
    # plt.xticks([5, 10, 15, 20])
    # ax1.ticklabel_format(axis="both", style="sci", scilimits=(0, 0))
    ax1.set_ylabel(r'Energy efficiency (bit/mJ)', fontsize=15)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw gridlines
    ax1.grid()

    # Save subplots to files
    filename = "bin1/Interval.svg"
    svg2emf(filename)

    # Show subplots
    plt.show()

    return
