import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter
from scipy import integrate

from src.tool.Avg import calAvgNum, getAvg
from src.tool.formatnum import formatnum, to_percent
from src.tool.Dataset import loadPDR, initCSJXNum

TX_INTERVAL = 10
pendTxLen = 28


def drawInstantPDR():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize number formatter
    scientificformatter = FuncFormatter(formatnum)
    percentageformatter = FuncFormatter(to_percent)

    # Init datasets
    CSJXNumDataset = initCSJXNum()

    # Load datasets
    (x1, y1) = loadPDR(CSJXNumDataset[1])
    (x2, y2) = loadPDR(CSJXNumDataset[2])
    (x3, y3) = loadPDR(CSJXNumDataset[3])
    (x4, y4) = loadPDR(CSJXNumDataset[4])

    # Calculate average PDR1
    (averagePDR1, averagePDRPoints1) = getAvg((x1, y1))

    # Calculate average PDR2
    (averagePDR2, averagePDRPoints2) = getAvg((x2, y2))

    # Calculate average PDR3
    (averagePDR3, averagePDRPoints3) = getAvg((x3, y3))

    # Calculate average PDR4
    (averagePDR4, averagePDRPoints4) = getAvg((x4, y4))

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
    ax1.plot(x1, y1, color='r', label=r'Instant (JX=1)')
    ax1.plot(x2, y2, color='y', label=r'Instant (JX=2)')
    ax1.plot(x3, y3, color='g', label=r'Instant (JX=3)')
    ax1.plot(x4, y4, color='b', label=r'Instant (JX=4)')

    ax1.plot(x1, averagePDRPoints1, color='r', linestyle="--", label=r'Average (JX=1)')
    ax1.plot(x2, averagePDRPoints2, color='y', linestyle="--", label=r'Average (JX=2)')
    ax1.plot(x3, averagePDRPoints3, color='g', linestyle="--", label=r'Average (JX=3)')
    ax1.plot(x4, averagePDRPoints4, color='b', linestyle="--", label=r'Average (JX=4)')

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw legends
    plt.legend(loc='best',
               ncol=2,
               fontsize=7)

    # Draw gridlines
    ax1.grid()

    # Draw title
    # plt.title(r'Instant Packet Deliver Ratio')

    # Save subplots to files
    plt.savefig("bin/PDR.pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return
