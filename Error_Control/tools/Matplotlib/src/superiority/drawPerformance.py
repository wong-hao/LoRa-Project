import matplotlib.pyplot as plt
import numpy as np

from scipy import integrate


def drawPM():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Draw one subplot
    fig, ax1 = plt.subplots()

    # Load GW1 data
    (x1, y1) = np.loadtxt('data/experimental/nonpower/GW/GWNum/1/error.csv', skiprows=1, delimiter=',', usecols=(0, 1), unpack=True)
    print(x1)
    print(y1)

    # Draw lines
    l1 = ax1.bar(x1, y1, color='r')

    # Save subplots to files
    plt.savefig("bin/CorrectionPerformance.pdf", format="pdf", transparent="ture")  # latex

    # Draw title
    plt.title(r'Packet Correction Performance')

    # Display subplots
    plt.show()
    return
