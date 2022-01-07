import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter
import matplotlib.ticker as mtick

from src.formatnum import formatnum


def drawData():
    formatter = FuncFormatter(formatnum)

    x1 = np.loadtxt('data/GatewayData.csv', delimiter=',', usecols=6, unpack=True)
    y1 = np.loadtxt('data/GatewayData.csv', delimiter=',', usecols=4, unpack=True)
    y2 = np.loadtxt('data/GatewayData.csv', delimiter=',', usecols=7, unpack=True)
    x2 = np.loadtxt('data/ServerData.csv', delimiter=',', usecols=0, unpack=True)
    y3 = np.loadtxt('data/ServerData.csv', delimiter=',', usecols=1, unpack=True)

    fig, ax1 = plt.subplots()
    ax2 = ax1.twinx()

    ax1.set_xlim((0, 2400000))
    ax1.set_ylim((0, 1))
    ax1.xaxis.set_major_formatter(formatter)
    ax1.set_xlabel(r'Time (ms)')
    ax1.set_ylabel(r'Packet delivery radio')

    ax2.set_xlim((0, 2400000))
    ax2.set_ylim((0, 0.017))
    ax2.set_ylabel(r'Throughout (kbp)', fontsize=7)

    l3, = ax1.plot(x1, y1, color='r')
    l4, = ax2.plot(x1, y2, color='b')
    l5, = ax2.plot(x2, y3, color='g')

    plt.legend(handles=[l3, l4, l5, ], labels=[r'Gateway PDR', r'Gateway Throughout', r'Server '
                                                                                      r'Throughout'],
               loc='best',

               fontsize=8)

    plt.savefig("bin/Data.pdf", format="pdf")

    plt.show()

    return
