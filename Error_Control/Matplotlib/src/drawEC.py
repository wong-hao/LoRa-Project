import matplotlib.pyplot as plt
import numpy as np


def drawEC():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Draw one subplot
    fig, ax1 = plt.subplots()

    # Load single point data
    x = np.loadtxt('data/power.csv', skiprows=1, delimiter=',', usecols=0, unpack=True)
    y1 = np.loadtxt('data/power.csv', skiprows=1, delimiter=',', usecols=2, unpack=True)
    y2 = np.loadtxt('data/power.csv', skiprows=1, delimiter=',', usecols=3, unpack=True)

    # Initialize axis
    ax1.set_xlabel(r'Time (s)')
    ax1.set_ylabel(r'Power (mW)')

    # Draw lines
    l1, = ax1.plot(x, y1*y2, color='r')

    # Draw a legend
    plt.legend(handles=[l1, ], labels=[r'RLoRa on'], loc='best')

    # Save subplots to files
    plt.savefig("bin/energy.pdf", format="pdf", transparent="ture")  # latex

    # Display subplots
    plt.show()
    return
