import matplotlib.pyplot as plt
import numpy as np


def drawSubplot():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Load data
    x = np.linspace(1, 8, 8)
    y1 = np.sin(x)
    y2 = np.linspace(1, 8, 8)
    y3 = np.linspace(8, 1, 8)
    y4 = np.linspace(0, 10, 256)
    y5 = np.linspace(3, 100, 256)

    # Initialize four subplots
    fig, ax = plt.subplots(2, 2)
    fig.tight_layout(h_pad=2)

    # Draw a line chart
    ax[0, 0].set_title('(a) SER-SNR Curves of Different SFs', y=-0.25)
    ax[0, 0].plot(x, y1, marker='*', markeredgecolor='r')
    ax[0, 0].minorticks_on()
    ax[0, 0].grid()

    # Draw a connection histogram
    ax[0, 1].set_title('(b) SNR Gains of Different SFs', y=-0.25)
    ax[0, 1].bar(x, y2)
    ax[0, 1].bar(x, y3, bottom=y2)

    # Draw a parallel histogram
    ax[1, 0].set_title('(c) BLG under SF=7, BW=125K', y=-0.25)
    width = 0.4
    before_half = x - width / 2
    after_half = x + width / 2
    ax[1, 0].bar(before_half, y2, width=width)
    ax[1, 0].bar(after_half, y3, width=width)
    ax[1, 0].set_xlim((0, 9))
    ax[1, 0].set_xticks([1, 2, 3, 4, 5, 6, 7, 8], ['SF5', 'SF6', 'SF7', 'SF8', 'SF9', 'SF10', 'SF11', 'SF12', ])

    for i in range(8):
        ax[1, 0].text(before_half[i], y2[i], '{:.1f}'.format(y2[i]), verticalalignment='bottom',
                      horizontalalignment='center')
        ax[1, 0].text(after_half[i], y3[i], '{:.1f}'.format(y3[i]), verticalalignment='bottom',
                      horizontalalignment='center')

    # Draw cumulative histogram
    ax[1, 1].set_xlim(0, 2 * max(max(y4), max(y5)))
    ax[1, 1].set_xlabel(r'Mean Energy Consumption (mJ)')
    ax[1, 1].set_ylabel(r'Cumulative Distribution Function (CDF)')

    ax[1, 1].hist(y4, density=True, histtype='step',
             cumulative=True, label='slight', color='b')

    ax[1, 1].annotate(r'%.1f' % max(y4), xy=(max(y4), 0), xycoords='data', xytext=(+30, +30),
                 textcoords='offset points', arrowprops=dict(arrowstyle='->', connectionstyle='arc3, rad=.2'))

    ax[1, 1].hist(y5, density=True, histtype='step',
             cumulative=True, label='serious', color='r')

    ax[1, 1].annotate(r'%.1f' % max(y5), xy=(max(y5), 0), xycoords='data', xytext=(+30, +30),
                 textcoords='offset points', arrowprops=dict(arrowstyle='->', connectionstyle='arc3, rad=.2'))


    # add overall title and adjust it so that it doesn't overlap with subplot titles
    # fig.suptitle('Overall Title')
    # plt.subplots_adjust(top=0.85)

    # Save subplots to files
    plt.savefig("bin/subplots.pdf", format="pdf", transparent="ture")  # latex

    # Show subplots
    plt.show()

    return
