import math

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter
import matplotlib.ticker as mtick


def drawSubplot():
    x = np.linspace(1, 8, 8)
    y1 = np.sin(x)
    y2 = np.linspace(1, 8, 8)
    y3 = np.linspace(8, 1, 8)

    # define subplots
    fig, ax = plt.subplots(2, 2)
    fig.tight_layout(h_pad=2)

    # define subplot titles
    ax[0, 0].set_title('(a) SER-SNR Curves of Different SFs', y=-0.25)
    ax[0, 0].plot(x, y1, marker='*', markeredgecolor='r')
    ax[0, 0].minorticks_on()
    ax[0, 0].grid()

    ax[0, 1].set_title('(b) SNR Gains of Different SFs', y=-0.25)
    ax[0, 1].bar(x, y2)
    ax[0, 1].bar(x, y3, bottom=y2)

    ax[1, 0].set_title('(c) BLG under SF=7, BW=125K', y=-0.25)
    width = 0.4
    before_half = x - width/2
    after_half = x + width/2
    ax[1, 0].bar(before_half, y2, width=width)
    ax[1, 0].bar(after_half, y3, width=width)

    ax[1, 0].set_xlim((0, 9))
    ax[1, 0].set_xticks([1, 2, 3, 4, 5, 6, 7, 8], ['SF5', 'SF6', 'SF7', 'SF8', 'SF9', 'SF10', 'SF11', 'SF12', ])

    for i in range(8):
        ax[1, 0].text(before_half[i], y2[i], '{:.1f}'.format(y2[i]), verticalalignment='bottom', horizontalalignment='center')
        ax[1, 0].text(after_half[i], y3[i], '{:.1f}'.format(y3[i]), verticalalignment='bottom', horizontalalignment='center')

    ax[1, 1].set_xlabel(r'Bit Location', y=-0.1)

    # add overall title and adjust it so that it doesn't overlap with subplot titles
    # fig.suptitle('Overall Title')
    # plt.subplots_adjust(top=0.85)

    plt.savefig("bin/subplots.pdf", format="pdf")  # latex

    # display subplots
    plt.show()

    return
