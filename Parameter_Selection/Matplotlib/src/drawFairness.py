import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter

from src.formatnum import formatnum, to_percent

def drawFairness():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize number formatter
    scientificformatter = FuncFormatter(formatnum)

    # Load data
    x = np.loadtxt('data/EE.csv', skiprows=1, delimiter=',', usecols=0, unpack=True)
    y1 = np.loadtxt('data/EE.csv', skiprows=1, delimiter=',', usecols=4, unpack=True)

    # Initialize two subplots
    fig, ax1 = plt.subplots()

    # Initialize axis
    ax1.xaxis.set_major_formatter(scientificformatter)
    ax1.set_xlabel(r'Time (ms)')
    ax1.set_ylabel('Jain\'s Fairness Index')

    # Draw lines
    l1, = ax1.plot(x, y1, color='r')

    # Draw gridlines
    ax1.grid()

    # Save subplots to files
    plt.savefig("bin/Fairness.pdf", format="pdf", transparent="ture")  # latex
    # plt.savefig("bin/EE.csv", format="csv") word

    # Show subplots
    plt.show()

    return