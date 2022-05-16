import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter
from scipy import integrate

from src.tool.calAvgNum import calAvgNum
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
    (x1, y1) = np.loadtxt('data/experimental/nonpower/GW/JXNum/1/data.csv', skiprows=1, delimiter=',', usecols=(6, 4), unpack=True)

    # Load GW2 data
    (x2, y2) = np.loadtxt('data/experimental/nonpower/GW/JXNum/2/data.csv', skiprows=1, delimiter=',', usecols=(6, 4), unpack=True)

    # Load GW3 data
    (x3, y3) = np.loadtxt('data/experimental/nonpower/GW/JXNum/3/data.csv', skiprows=1, delimiter=',', usecols=(6, 4), unpack=True)

    # Load GW4 data
    (x4, y4) = np.loadtxt('data/experimental/nonpower/GW/JXNum/4/data.csv', skiprows=1, delimiter=',', usecols=(6, 4), unpack=True)

    # Calculate average PDR1
    averagePDR1 = calAvgNum(y1, x1)
    averagePDRPoints1 = np.linspace(averagePDR1, averagePDR1, len(x1))

    # Calculate average PDR2
    averagePDR2 = calAvgNum(y2, x2)
    averagePDRPoints2 = np.linspace(averagePDR2, averagePDR2, len(x2))

    # Calculate average PDR3
    averagePDR3 = calAvgNum(y3, x3)
    averagePDRPoints3 = np.linspace(calAvgNum(y3, x3), calAvgNum(y3, x3), len(x3))

    # Calculate average PDR4
    averagePDR4 = calAvgNum(y4, x4)
    averagePDRPoints4 = np.linspace(averagePDR4, averagePDR4, len(x4))

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
    l2, = ax1.plot(x2, y2, color='y')
    l3, = ax1.plot(x3, y3, color='g')
    l4, = ax1.plot(x4, y4, color='b')

    l5, = ax1.plot(x1, averagePDRPoints1, color='r', linestyle="--")
    l6, = ax1.plot(x2, averagePDRPoints2, color='y', linestyle="--")
    l7, = ax1.plot(x3, averagePDRPoints3, color='g', linestyle="--")
    l8, = ax1.plot(x4, averagePDRPoints4, color='b', linestyle="--")

    # Draw legends
    plt.legend(handles=[l1, l2, l3, l4, l5, l6, l7, l8, ], labels=[r'Instant (JX=1)', r'Instant (JX=2)', r'Instant (JX=3)', r'Instant '
                                                                                                            r'(JX=4)', r'Average (JX=1)', r'Average (JX=2)', r'Average (JX=3)', r'Average (JX=4)'],
               loc='best',

               ncol=2,

               fontsize=8)

    # Draw gridlines
    ax1.grid()

    # Draw title
    # plt.title(r'Instant Packet Deliver Ratio')

    # Save subplots to files
    plt.savefig("bin/PDR.pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return
