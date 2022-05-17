import numpy as np
from matplotlib import pyplot as plt

# http://t.csdn.cn/jxmCD
# https://www.jb51.net/article/206226.htm

# Draw a parallel histogram
def drawEffThroughput():

    # Choose font
    plt.rc('font', family='Times New Roman')

    # x为每组柱子x轴的基准位置
    labels = ['SF=7', 'SF=8', 'SF=9', 'SF=10', 'SF=11', 'SF=12']

    # Load data
    zero = [20, 34, 30, 35, 27, 11]
    first = [20, 34, 30, 35, 27, 11]
    second = [25, 32, 34, 20, 25, 11]
    third = [21, 31, 37, 21, 28, 11]
    fourth = [26, 31, 35, 27, 21, 11]
    fifth = [26, 31, 35, 27, 21, 11]
    sixth = [26, 31, 35, 27, 21, 11]
    seventh = [26, 31, 35, 27, 21, 11]
    datas = [zero, first, second, third, fourth, fifth, sixth, seventh]

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
        plt.bar(x + index * bar_span, y, bar_width, label='TP='+str(index))

    ax1.set_ylabel('Throughput (kbps)')

    # ticks为新x轴刻度标签位置，即每组柱子x轴上的中心位置
    ticks = x + (group_width - bar_span) / 2
    plt.xticks(ticks, labels)

    # Draw legends
    plt.legend()

    # Draw gridlines
    ax1.grid()

    plt.savefig("bin/ABC.pdf", format="pdf", transparent="ture")

    plt.show()
