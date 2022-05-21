import matplotlib.pyplot as plt
import numpy as np


def drawSupCSPDR():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Load data
    x = np.linspace(0, 6, 6)
    y1 = np.sin(x)
    y2 = np.cos(x)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Draw two line charts
    # ax1.plot(x, y1, marker='*', markeredgecolor='r', label='OPR')
    plt.errorbar(x, y1, yerr=0.2, ecolor='r', color='b', capsize=4, marker='x', markersize=15, markeredgecolor='b', label='CLoRa')
    ax1.errorbar(x, y2, yerr=0.2, ecolor='r', color='g', capsize=4, marker='o', markersize=15, markeredgecolor='g', label='CLoRa')

    # Draw gridlines
    ax1.grid()

    # Draw legends
    plt.legend(fontsize=15)
    # Draw title
    # plt.title(r'Instant Packet Deliver Ratio')

    # Initialize axis
    ax1.set_xlabel(r'Number of gateways', fontsize=15)
    ax1.set_ylabel(r'Packet Delivery Ratio', fontsize=15)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Save subplots to files
    plt.savefig("bin/SupCSPDR.pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return
