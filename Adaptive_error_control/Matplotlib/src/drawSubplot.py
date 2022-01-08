import math

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter
import matplotlib.ticker as mtick


def drawSubplot():
    x = np.linspace(0, 10, 20)
    y1 = np.sin(x)
    y2 = np.linspace(0, 0, 20)

    # define subplots
    fig, ax = plt.subplots(2, 2)
    fig.tight_layout(h_pad=2)

    # define subplot titles
    ax[0, 0].set_title('(a) SER-SNR Curves of Different SFs', y=-0.25)
    ax[0, 0].plot(x, y1, marker='*', markeredgecolor='r')
    ax[0, 0].minorticks_on()
    ax[0, 0].grid()

    ax[1, 0].set_title('(b) SNR Gains of Different SFs', y=-0.25)
    ax[1, 0].set_title('(c) BLG under SF=7, BW=125K', y=-0.25)
    ax[1, 1].set_xlabel(r'Bit Location', y=-0.1)

    # add overall title and adjust it so that it doesn't overlap with subplot titles
    # fig.suptitle('Overall Title')
    # plt.subplots_adjust(top=0.85)

    plt.savefig("bin/subplots.pdf", format="pdf")  # latex

    # display subplots
    plt.show()

    return
