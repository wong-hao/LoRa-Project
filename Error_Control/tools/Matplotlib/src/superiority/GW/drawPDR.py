import matplotlib.pyplot as plt
import numpy as np

def drawSupGWPDR():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Load data
    x = np.linspace(0, 6, 6)
    y1 = np.sin(x)
    y2 = np.cos(x)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Draw two line charts
    ax1.plot(x, y1, marker='*', markeredgecolor='r', label='OPR')
    ax1.plot(x, y2, marker='+', markeredgecolor='r', label='CLoRa')

    # Draw gridlines
    ax1.grid()

    # Draw legends
    plt.legend(fontsize=15)

    # Draw title
    # plt.title(r'Instant Packet Deliver Ratio')

    # Initialize axis
    ax1.set_xlabel(r'Number of gateways', fontsize=15)
    ax1.set_ylabel(r'Throughput (kbps)', fontsize=15)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Save subplots to files
    plt.savefig("bin/SupGWPDR.pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return
