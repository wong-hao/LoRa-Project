import matplotlib.pyplot as plt
import numpy as np


def drawEnergyEfficiency():
    fig, ax1 = plt.subplots()

    y1 = np.linspace(0, 10, 256)
    y2 = np.linspace(3, 100, 256)

    ax1.set_xlim(0, 2*max(max(y1), max(y2)))
    ax1.set_xlabel(r'Mean Energy Consumption (mJ)')
    ax1.set_ylabel(r'Cumulative Distribution Function (CDF)')

    # plot the cumulative histogram
    ax1.hist(y1, density=True, histtype='step',
             cumulative=True, label='slight', color='b')

    ax1.annotate(r'%.1f' % max(y1), xy=(max(y1), 0), xycoords='data', xytext=(+30, +30),
                 textcoords='offset points', arrowprops=dict(arrowstyle='->', connectionstyle='arc3, rad=.2'))

    ax1.hist(y2, density=True, histtype='step',
             cumulative=True, label='serious', color='r')

    ax1.annotate(r'%.1f' % max(y2), xy=(max(y2), 0), xycoords='data', xytext=(+30, +30),
                 textcoords='offset points', arrowprops=dict(arrowstyle='->', connectionstyle='arc3, rad=.2'))

    plt.legend(loc='best')

    plt.savefig("bin/energy.pdf", format="pdf")  # latex

    # display subplots
    plt.show()
    return
