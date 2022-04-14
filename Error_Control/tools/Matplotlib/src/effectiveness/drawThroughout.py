import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter

from src.tool.formatnum import formatnum, to_percent, formatnum2

TX_INTERVAL = 10
pendTxLen = 28


def drawThroughput():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize number formatter
    scientificformatter = FuncFormatter(formatnum)
    scientificformatter2 = FuncFormatter(formatnum2)

    # Theoretical upper limit of throughput
    # theory = pendTxLen * 8 / (TX_INTERVAL * 1000)

    # Load GW1 data
    x1 = np.loadtxt('data/experimental/nonpower/1/GW.csv', skiprows=1, delimiter=',', usecols=6, unpack=True)
    y1 = np.loadtxt('data/experimental/nonpower/1/GW.csv', skiprows=1, delimiter=',', usecols=7, unpack=True)
    # y1 = y1 / theory

    # Load NS1 data
    x2 = np.loadtxt('data/experimental/nonpower/1/NS.csv', skiprows=1, delimiter=',', usecols=0, unpack=True)
    y2 = np.loadtxt('data/experimental/nonpower/1/NS.csv', skiprows=1, delimiter=',', usecols=1, unpack=True)
    # y2 = y2 / theory

    # TODO: Load data when RLoRa off
    x3 = np.loadtxt('data/control/1/GW.csv', skiprows=1, delimiter=',', usecols=6, unpack=True)
    y3 = np.loadtxt('data/control/1/GW.csv', skiprows=1, delimiter=',', usecols=7, unpack=True)
    # y3 = y3 / theory
    x4 = np.loadtxt('data/control/1/NS.csv', skiprows=1, delimiter=',', usecols=0, unpack=True)
    y4 = np.loadtxt('data/control/1/NS.csv', skiprows=1, delimiter=',', usecols=1, unpack=True)
    # y4 = y4 / theory

    # Initialize subplot
    fig, ax1 = plt.subplots(figsize=(8, 6))

    # Initialize axis
    # ax1.set_ylim((0, 1))
    ax1.xaxis.set_major_formatter(scientificformatter)
    ax1.yaxis.set_major_formatter(scientificformatter2)
    ax1.set_xlabel(r'Time (ms)')
    ax1.set_ylabel(r'Throughput (kbps)', fontsize=7)

    # Draw lines
    l1, = ax1.plot(x1, y1, color='r')
    l2, = ax1.plot(x2, y2, color='b', linestyle="--")
    l3, = ax1.plot(x3, y3, color='g')
    l4, = ax1.plot(x4, y4, color='y', linestyle="--")

    # Draw legends
    plt.legend(handles=[l1, l2, l3, l4, ], labels=[r'GW: on', r'NS: on', r'GW: off', r'NS: off'],
               loc='best',

               fontsize=8)

    # Draw gridlines
    ax1.grid()

    # Draw title
    plt.title(r'Instant Throughput')

    # Save subplots to files
    plt.savefig("bin/Throughput.pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return
