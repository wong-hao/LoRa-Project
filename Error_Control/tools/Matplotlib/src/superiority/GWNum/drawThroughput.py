import matplotlib.pyplot as plt
import numpy as np


def drawSupCSThroughput():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize datasets
    x = np.linspace(1, 8, 8)
    y1 = np.sin(x)

    # Initialize subplots
    fig, ax1 = plt.subplots()

    # Draw a line chart
    ax1.set_title('(a) SER-SNR Curves of Different SFs', y=-0.25)
    ax1.plot(x, y1, marker='*', markeredgecolor='r')
    ax1.minorticks_on()
    ax1.tick_params(labelsize=15)

    # Draw gridlines
    ax1.grid()

    # Draw title
    # plt.title(r'Instant Packet Deliver Ratio')

    # Save subplots to files
    plt.savefig("bin/PDR.pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return
