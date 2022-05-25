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

    # Draw two error charts
    yerr = [0.2, 0.18, 0.16, 0.14, 0.12, 0.1]
    plt.errorbar(x, y1, yerr=yerr, ecolor='black', color='b', capsize=4, marker='x', markersize=7, markeredgecolor='b', label='CLoRa')
    ax1.errorbar(x, y2, yerr=yerr, ecolor='black', color='r', capsize=4, marker='o', markersize=7, markeredgecolor='r', label='OPR')

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
    plt.savefig("bin/PDR(GWNum).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return
