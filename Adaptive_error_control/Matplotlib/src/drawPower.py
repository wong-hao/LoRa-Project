import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter
import matplotlib.ticker as mtick


def drawPower():
    x = np.loadtxt('data/power.csv', delimiter=',', usecols=0, unpack=True)
    y1 = np.loadtxt('data/power.csv', delimiter=',', usecols=3, unpack=True)
    y2 = np.loadtxt('data/power.csv', delimiter=',', usecols=2, unpack=True)

    fig, ax1 = plt.subplots()
    ax2 = ax1.twinx()

    # plt.xticks(np.linspace(7, 11, 10))
    # plt.yticks(np.linspace(0, 200, 10))

    ax1.set_xlim((7, 11))
    ax1.set_ylim((0, 200))
    ax1.set_xlabel(r'Time (s)')
    ax1.set_ylabel(r'USB Avg Current (mA)')

    ax2.set_xlim((7, 11))
    ax2.set_ylim((4, 5.5))
    ax2.set_ylabel(r'Main Avg Voltage (V)')

    l1, = ax1.plot(x, y1, color='r')
    l2, = ax2.plot(x, y2, color='b')

    plt.legend(handles=[l1, l2, ], labels=[r'USB Avg Current', r'Main Avg Voltage'], loc='best')

    plt.savefig("bin/power.pdf", format="pdf")  # latex
    # plt.savefig("bin/power.csv", format="csv") word
    plt.show()

    return
