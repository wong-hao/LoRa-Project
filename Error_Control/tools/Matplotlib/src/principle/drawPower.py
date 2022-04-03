import matplotlib.pyplot as plt
import numpy as np


def drawPower():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Load data
    x = np.loadtxt('data/experimental/1/power.csv', skiprows=1, delimiter=',', usecols=0, unpack=True)
    y1 = np.loadtxt('data/experimental/1/power.csv', skiprows=1, delimiter=',', usecols=3, unpack=True)
    y2 = np.loadtxt('data/experimental/1/power.csv', skiprows=1, delimiter=',', usecols=2, unpack=True)

    # Initialize two subplots
    fig, ax1 = plt.subplots()
    ax2 = ax1.twinx()

    # Initialize subplot1 axis
    ax1.set_xlim((7, 11))
    ax1.set_ylim((0, 200))
    ax1.set_xlabel(r'Time (s)')
    ax1.set_ylabel(r'USB Avg Current (mA)')

    # Initialize subplot2 axis
    ax2.set_ylim((4, 5.5))
    ax2.set_ylabel(r'Main Avg Voltage (V)')

    # Draw lines
    l1, = ax1.plot(x, y1, color='r')
    l2, = ax2.plot(x, y2, color='b')

    # Draw legends
    plt.legend(handles=[l1, l2, ], labels=[r'USB Avg Current', r'Main Avg Voltage'], loc='best')

    # Draw gridlines
    ax1.grid()

    # Save subplots to files
    plt.savefig("bin/power.pdf", format="pdf", transparent="ture")  # latex
    # plt.savefig("bin/power.csv", format="csv") word

    # Show subplots
    plt.show()

    return