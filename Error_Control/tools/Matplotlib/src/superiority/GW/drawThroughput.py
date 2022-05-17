import matplotlib.pyplot as plt
import numpy as np


def drawSupGWThroughput():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Load data
    x = np.linspace(1, 8, 8)
    y1 = np.sin(x)

    # Initialize two subplots
    fig, ax1 = plt.subplots()

    # Draw a line chart
    ax1.set_title('(a) SER-SNR Curves of Different SFs', y=-0.25)
    ax1.plot(x, y1, marker='*', markeredgecolor='r')
    ax1.minorticks_on()
    ax1.grid()

    # Draw gridlines
    ax1.grid()

    # Draw title
    # plt.title(r'Instant Packet Deliver Ratio')

    # Save subplots to files
    plt.savefig("bin/PDR.pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return
