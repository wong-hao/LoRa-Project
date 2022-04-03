import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter

from src.tool.formatnum import formatnum


def drawEE():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize number formatter
    scientificformatter = FuncFormatter(formatnum)

    # Load data
    x = np.loadtxt('data/experimental/1/ADR.csv', skiprows=1, delimiter=',', usecols=0, unpack=True)
    y1 = np.loadtxt('data/experimental/1/ADR.csv', skiprows=1, delimiter=',', usecols=2, unpack=True)
    y2 = np.loadtxt('data/experimental/1/ADR.csv', skiprows=1, delimiter=',', usecols=3, unpack=True)
    y3 = np.loadtxt('data/experimental/1/EEADR.csv', skiprows=1, delimiter=',', usecols=2, unpack=True)
    y4 = np.loadtxt('data/experimental/1/EEADR.csv', skiprows=1, delimiter=',', usecols=3, unpack=True)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Initialize axis
    ax1.xaxis.set_major_formatter(scientificformatter)
    ax1.set_xlabel(r'Time (ms)')
    ax1.set_ylabel(r'Energy Efficiency (bit/mJ)')

    # Draw lines
    l1, = ax1.plot(x, y1, color='r', linestyle='dashed')
    l2, = ax1.plot(x, y2, color='b', linestyle='dashed')
    l3, = ax1.plot(x, y3, color='g')
    l4, = ax1.plot(x, y4, color='y')

    # Draw legends
    ax1.legend(handles=[l1, l2, l3, l4], labels=[r'Node1-ADR', r'Node2-ADR', r'Node1-EEADR', r'Node2-EEADR'],
               loc='best')

    # Draw gridlines
    ax1.grid()

    # Save subplots to files
    plt.savefig("bin/EE.pdf", format="pdf", transparent="ture")  # latex
    # plt.savefig("bin/EE.csv", format="csv") word

    # Show subplots
    plt.show()

    return
