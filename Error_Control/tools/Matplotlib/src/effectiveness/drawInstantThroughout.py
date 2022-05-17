import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter
from scipy import integrate

from src.tool.calAvgNum import calAvgNum
from src.tool.formatnum import formatnum, to_percent, formatnum2
from src.tool.loadThroughput import loadGWTHroughput, loadNSTHroughput

TX_INTERVAL = 10
pendTxLen = 28


def drawInstantThroughput():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize number formatter
    scientificformatter = FuncFormatter(formatnum)
    scientificformatter2 = FuncFormatter(formatnum2)

    # Theoretical upper limit of throughput
    # theory = pendTxLen * 8 / (TX_INTERVAL * 1000)

    # Load GW1 data
    (x1, y1) = loadGWTHroughput('data/experimental/nonpower/GW/JXNum/3/data.csv')

    # Load NS1 data
    (x2, y2) = loadNSTHroughput('data/experimental/nonpower/NS/JXNum/3/data.csv')

    # Load GW2 data
    (x3, y3) = loadGWTHroughput('data/control/GW/1/data.csv')

    # Load NS2 data
    (x4, y4) = loadNSTHroughput('data/control/NS/1/data.csv')

    # Calculate average throughput1
    averagethroughput1 = calAvgNum(y1, x1)
    averagethroughputPoints1 = np.linspace(averagethroughput1, averagethroughput1, len(x1))

    # Calculate average throughput2
    averagethroughput2 = calAvgNum(y2, x2)
    averagethroughputPoints2 = np.linspace(averagethroughput2, averagethroughput2, len(x2))

    # Calculate average throughput3
    averagethroughput3 = calAvgNum(y3, x3)
    averagethroughputPoints3 = np.linspace(averagethroughput3, averagethroughput3, len(x3))

    # Calculate average throughput4
    averagethroughput4 = calAvgNum(y4, x4)
    averagethroughputPoints4 = np.linspace(averagethroughput4, averagethroughput4, len(x4))

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Initialize axis
    # ax1.set_ylim((0, 1))
    # ax1.xaxis.set_major_formatter(scientificformatter)
    # ax1.yaxis.set_major_formatter(scientificformatter2)
    ax1.ticklabel_format(axis="both", style="sci", scilimits=(0, 0))
    ax1.set_xlabel(r'Time (ms)')
    ax1.set_ylabel(r'Throughput (kbps)', fontsize=7)

    # Draw lines
    l1, = ax1.plot(x1, y1, color='r')
    l2, = ax1.plot(x2, y2, color='b')
    l3, = ax1.plot(x3, y3, color='g')
    l4, = ax1.plot(x4, y4, color='y')

    l5, = ax1.plot(x1, averagethroughputPoints1, color='r', linestyle="--")
    l6, = ax1.plot(x2, averagethroughputPoints2, color='b', linestyle="--")
    l7, = ax1.plot(x3, averagethroughputPoints3, color='g', linestyle="--")
    l8, = ax1.plot(x4, averagethroughputPoints4, color='y', linestyle="--")

    # Draw legends
    plt.legend(handles=[l1, l2, l3, l4, l5, l6, l7, l8, ], labels=[r'Instant (GW: disabled)', r'Instant (NS: disabled)', r'Instant (GW: enabled)', r'Instant (NS: enabled)', r'Average (GW: disabled)', r'Average (NS: disabled)', r'Average (GW: enabled)', r'Average (NS: enabled)'],
               loc='best',
               fontsize=8,
               ncol=2)

    # Draw gridlines
    ax1.grid()

    # Draw title
    # plt.title(r'Instant Throughput')

    # Save subplots to files
    plt.savefig("bin/Throughput.pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return
