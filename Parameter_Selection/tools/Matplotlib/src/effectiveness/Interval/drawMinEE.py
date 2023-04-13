import numpy as np
from matplotlib import pyplot as plt

from src.tool.dataset import initInterval, IntervalLevel, loadFinalMinEE
from src.tool.formatfile import svg2emf


def drawIntervalMinEE():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize datasets
    Intervaldataset = initInterval()

    # Load datasets
    x = np.linspace(5000, 20000, 4)
    y = []

    for loopcount in range(IntervalLevel):

        minEE = loadFinalMinEE(Intervaldataset[loopcount])
        y.append(minEE)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Draw one line
    ax1.plot(x, y, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='EEADR')

    # Initialize axis
    # plt.xticks([1, 2, 3, 4], ['Tg=5', 'Tg=10', 'Interval=15', 'Interval=20'])
    ax1.set_xlabel(r'Packet transmission interval (ms)', fontsize=15)
    plt.xticks([5000, 10000, 15000, 20000])
    ax1.ticklabel_format(axis="both", style="sci", scilimits=(0, 0))
    ax1.set_ylabel(r'Energy Efficiency (bit/mJ)', fontsize=15)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw gridlines
    ax1.grid()

    # Save subplots to files
    filename = "bin/Interval.svg"
    svg2emf(filename)

    # Show subplots
    plt.show()

    return
