import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter

from src.tool.formatnum import formatnum, to_percent

TX_INTERVAL = 10
pendTxLen = 28


def drawPDR():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize number formatter
    scientificformatter = FuncFormatter(formatnum)
    percentageformatter = FuncFormatter(to_percent)

    # Load GW1 data
    x1 = np.loadtxt('data/experimental/nonpower/GW/JXNum/1/data.csv', skiprows=1, delimiter=',', usecols=6, unpack=True)
    y1 = np.loadtxt('data/experimental/nonpower/GW/JXNum/1/data.csv', skiprows=1, delimiter=',', usecols=4, unpack=True)

    # Load GW2 data
    x2 = np.loadtxt('data/experimental/nonpower/GW/JXNum/2/data.csv', skiprows=1, delimiter=',', usecols=6, unpack=True)
    y2 = np.loadtxt('data/experimental/nonpower/GW/JXNum/2/data.csv', skiprows=1, delimiter=',', usecols=4, unpack=True)

    # Load GW3 data
    x3 = np.loadtxt('data/experimental/nonpower/GW/JXNum/3/data.csv', skiprows=1, delimiter=',', usecols=6, unpack=True)
    y3 = np.loadtxt('data/experimental/nonpower/GW/JXNum/3/data.csv', skiprows=1, delimiter=',', usecols=4, unpack=True)

    # Load GW4 data
    x4 = np.loadtxt('data/experimental/nonpower/GW/JXNum/4/data.csv', skiprows=1, delimiter=',', usecols=6, unpack=True)
    y4 = np.loadtxt('data/experimental/nonpower/GW/JXNum/4/data.csv', skiprows=1, delimiter=',', usecols=4, unpack=True)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Initialize axis
    ax1.set_xlim((0, 2400000))
    ax1.set_ylim((0, 1.05))
    # ax1.xaxis.set_major_formatter(scientificformatter)
    ax1.ticklabel_format(axis="x", style="sci", scilimits=(0, 0))
    ax1.yaxis.set_major_formatter(percentageformatter)
    ax1.set_xlabel(r'Time (ms)')
    ax1.set_ylabel(r'Packet Delivery Ratio (%)')

    # Draw lines
    l1, = ax1.plot(x1, y1, color='r')
    l2, = ax1.plot(x2, y2, color='y', linestyle="--")
    l3, = ax1.plot(x3, y3, color='g', linestyle="--")
    l4, = ax1.plot(x4, y4, color='b', linestyle="--")

    # Draw legends
    plt.legend(handles=[l1, l2, l3, l4, ], labels=[r'JX=1', r'JX=2', r'JX=3', r'JX=4'],
               loc='best',

               fontsize=8)

    # Draw gridlines
    ax1.grid()

    # Draw title
    plt.title(r'Instant Packet Deliver Ratio')

    # Save subplots to files
    plt.savefig("bin/PDR.pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return
