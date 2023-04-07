import matplotlib.pyplot as plt
import numpy as np

def drawPower():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize dataset
    (x, y1, y2) = np.loadtxt('data/show/power.csv', skiprows=1, delimiter=',', usecols=(0, 10, 3), unpack=True)

    # Initialize subplots
    fig, ax1 = plt.subplots()
    ax2 = ax1.twinx()

    # Initialize subplot1 axis
    ax1.set_xlim((7.5, 12))
    ax1.set_ylim((0, 200))
    ax1.set_xlabel(r'Time (s)')
    ax1.set_ylabel(r'Current (mA)')

    # Initialize subplot2 axis
    ax2.set_ylim((4, 5.5))
    ax2.set_ylabel(r'Voltage (V)')

    # Draw lines
    l1, = ax1.plot(x, y1, color='r')
    l2, = ax2.plot(x, y2, color='b')

    # Draw legends
    plt.legend(handles=[l1, l2, ], labels=[r'Current', r'Voltage'], loc='best')

    # Draw gridlines
    ax1.grid()

    # Draw title
    # plt.title(r'Measured Power Data')

    # Save subplots to files
    plt.savefig("bin/power.svg", format="svg", transparent="ture")  # latex
    # plt.savefig("bin/power.csv", format="csv") word

    # Show subplots
    plt.show()

    return
