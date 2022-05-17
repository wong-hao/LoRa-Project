import matplotlib.pyplot as plt
import numpy as np

def drawSupGWPDR():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Load data
    x = np.linspace(3, 6, 4)
    y1 = np.sin(x)
    y2 = np.cos(x)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Draw two line charts
    ax1.plot(x, y1, marker='*', markeredgecolor='r', label='OPR')
    ax1.plot(x, y2, marker='*', markeredgecolor='r', label='CLoRa')
    ax1.minorticks_on()

    # Draw gridlines
    ax1.grid()

    # Draw legends
    plt.legend()

    # Draw title
    # plt.title(r'Instant Packet Deliver Ratio')

    # Initialize axis
    ax1.set_xlabel(r'Number of gateways')
    ax1.set_ylabel(r'Throughput (kbps)', fontsize=7)

    # Save subplots to files
    plt.savefig("bin/SupGWPDR.pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return
