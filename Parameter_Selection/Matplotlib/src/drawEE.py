import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter

from src.formatnum import formatnum, to_percent

def drawEE():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize number formatter
    scientificformatter = FuncFormatter(formatnum)

    # Load data
    x = np.loadtxt('data/EE.csv', skiprows=1, delimiter=',', usecols=0, unpack=True)
    y1 = np.loadtxt('data/EE.csv', skiprows=1, delimiter=',', usecols=2, unpack=True)
    y2 = np.loadtxt('data/EE.csv', skiprows=1, delimiter=',', usecols=3, unpack=True)

    # Initialize two subplots
    fig, ax1 = plt.subplots()

    # Initialize axis
    ax1.xaxis.set_major_formatter(scientificformatter)
    ax1.set_xlabel(r'Time (ms)')
    ax1.set_ylabel(r'Energy Efficiency (bit/j)')

    # Draw lines
    l1, = ax1.plot(x, y1, color='r')
    l2, = ax1.plot(x, y2, color='b')

    # Draw legends
    ax1.legend(handles=[l1, l2, ], labels=[r'Node1', r'Node2'], loc='best')

    # Draw gridlines
    ax1.grid()

    # Save subplots to files
    plt.savefig("bin/EE.pdf", format="pdf", transparent="ture")  # latex
    # plt.savefig("bin/EE.csv", format="csv") word

    # Show subplots
    plt.show()

    return