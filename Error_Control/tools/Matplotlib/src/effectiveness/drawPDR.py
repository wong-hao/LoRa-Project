import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter

from src.tool.formatnum import formatnum, to_percent

TX_INTERVAL = 20
pendTxLen = 28


def drawPDR():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize number formatter
    scientificformatter = FuncFormatter(formatnum)

    # Load GW data
    x1 = np.loadtxt('data/experimental/1/GatewayData.csv', skiprows=1, delimiter=',', usecols=6, unpack=True)
    y1 = np.loadtxt('data/experimental/1/GatewayData.csv', skiprows=1, delimiter=',', usecols=4, unpack=True)

    # Load NS data
    x2 = np.loadtxt('data/experimental/1/ServerData.csv', skiprows=1, delimiter=',', usecols=0, unpack=True)
    y2 = np.loadtxt('data/experimental/1/ServerData.csv', skiprows=1, delimiter=',', usecols=1, unpack=True)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Initialize axis
    ax1.set_xlim((0, 2400000))
    ax1.set_ylim((0, 1))
    ax1.xaxis.set_major_formatter(scientificformatter)
    ax1.set_xlabel(r'Time (ms)')
    ax1.set_ylabel(r'Packet delivery ratio')

    # Draw lines
    l1, = ax1.plot(x1, y1, color='r')
    l2, = ax1.plot(x2, y2, color='b', linestyle="--")

    # Draw legends
    plt.legend(handles=[l1, l2, ], labels=[r'PDR(GW)', r'PDR(NS)', ],
               loc='best',

               fontsize=8)

    # Draw gridlines
    ax1.grid()

    # Save subplots to files
    plt.savefig("bin/PDR.pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return
