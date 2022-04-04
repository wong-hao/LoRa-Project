import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter

from src.tool.formatnum import formatnum, to_percent

TX_INTERVAL = 20
pendTxLen = 28


def drawThroughout():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize number formatter
    scientificformatter = FuncFormatter(formatnum)
    percentageformatter = FuncFormatter(to_percent)

    # Theoretical upper limit of throughput
    theory = pendTxLen * 8 / (TX_INTERVAL * 1000)

    # Load GW data
    x1 = np.loadtxt('data/experimental/1/GatewayData.csv', skiprows=1, delimiter=',', usecols=6, unpack=True)
    y1 = np.loadtxt('data/experimental/1/GatewayData.csv', skiprows=1, delimiter=',', usecols=7, unpack=True)
    y1 = y1 / theory

    # Load NS data
    x2 = np.loadtxt('data/experimental/1/ServerData.csv', skiprows=1, delimiter=',', usecols=0, unpack=True)
    y2 = np.loadtxt('data/experimental/1/ServerData.csv', skiprows=1, delimiter=',', usecols=1, unpack=True)
    y2 = y2 / theory

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Initialize axis
    ax1.set_ylim((0, 1))
    ax1.xaxis.set_major_formatter(scientificformatter)
    ax1.yaxis.set_major_formatter(percentageformatter)
    ax1.set_xlabel(r'Time (ms)')
    ax1.set_ylabel(r'Throughout ratio', fontsize=7)

    # Draw lines
    l1, = ax1.plot(x1, y1, color='r')
    l2, = ax1.plot(x2, y2, color='b', linestyle="--")

    # Draw legends
    plt.legend(handles=[l1, l2, ], labels=[r'Throughout(GW)', r'Throughout(NS)', ],
               loc='best',

               fontsize=8)

    # Draw gridlines
    ax1.grid()

    # Save subplots to files
    plt.savefig("bin/Throughout.pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return
