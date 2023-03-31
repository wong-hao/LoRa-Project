import numpy as np
from matplotlib import pyplot as plt

from src.tool.dataset import initInterval, IntervalLevel, loadFinalMinEE


def drawIntervalMinEE():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize datasets
    Intervaldataset = initInterval()

    # Load datasets
    x = np.linspace(1, IntervalLevel, IntervalLevel)
    print(x)
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
    plt.xticks([1, 2, 3, 4], ['Interval=5', 'Interval=5', 'Interval=15', 'Interval=20'])
    ax1.set_ylabel(r'Energy Efficiency (bit/mJ)', fontsize=15)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw gridlines
    ax1.grid()

    # Save subplots to files
    plt.savefig("bin/Interval.svg", format="svg", transparent="ture")

    # Show subplots
    plt.show()

    return
