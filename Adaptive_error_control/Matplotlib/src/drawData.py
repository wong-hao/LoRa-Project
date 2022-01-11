import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter

from src.formatnum import formatnum, to_percent

TX_INTERVAL = 20
pendTxLen = 28


def drawData():
    formatter = FuncFormatter(formatnum)

    theory = pendTxLen * 8 / (TX_INTERVAL * 1000)

    x1 = np.loadtxt('data/GatewayData.csv', delimiter=',', usecols=6, unpack=True)
    y1 = np.loadtxt('data/GatewayData.csv', delimiter=',', usecols=4, unpack=True)
    y2 = np.loadtxt('data/GatewayData.csv', delimiter=',', usecols=7, unpack=True)
    y2 = y2 / theory
    x2 = np.loadtxt('data/ServerData.csv', delimiter=',', usecols=0, unpack=True)
    y3 = np.loadtxt('data/ServerData.csv', delimiter=',', usecols=1, unpack=True)
    y3 = y3 / theory

    fig, ax1 = plt.subplots()
    ax2 = ax1.twinx()

    ax1.set_xlim((0, 2400000))
    ax1.set_ylim((0, 1))
    ax1.xaxis.set_major_formatter(formatter)
    ax1.set_xlabel(r'Time (ms)')
    ax1.set_ylabel(r'Packet delivery ratio')

    ax2.set_xlim((0, 2400000))
    ax2.set_ylim((0, 1))
    ax2.yaxis.set_major_formatter(FuncFormatter(to_percent))
    ax2.set_ylabel(r'Throughout ratio', fontsize=7)

    l1, = ax1.plot(x1, y1, color='r')
    l2, = ax2.plot(x1, y2, color='b', linestyle="--")
    l3, = ax2.plot(x2, y3, color='g', linestyle="-.")

    ax2.plot([x1[int(len(x1)/2)], x1[int(len(x1)/2)]], [y1[int(len(y1)/2)], y2[int(len(y2)/2)]], color='black', linestyle="--")

    ax2.annotate(r'Latency offset', xy=(x1[int(len(x1)/2)], (y1[int(len(y1)/2)]+y2[int(len(y2)/2)])/2), xycoords='data', xytext=(+30, -30),
                 textcoords='offset points', arrowprops=dict(arrowstyle='->', connectionstyle='arc3, rad=.2'))

    plt.legend(handles=[l1, l2, l3, ], labels=[r'Gateway PDR', r'Gateway Throughout', r'Server '
                                                                                      r'Throughout'],
               loc='best',

               fontsize=8)

    plt.savefig("bin/Data.pdf", format="pdf")

    plt.grid()

    plt.show()

    return
