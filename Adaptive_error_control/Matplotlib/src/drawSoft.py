import math

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter
import matplotlib.ticker as mtick


def drawSoft():
    x = np.linspace(0, 10, 256)
    y1 = np.sin(x)
    y2 = np.linspace(0, 0, 256)

    fig, ax1 = plt.subplots()
    ax2 = ax1.twinx()

    ax1.set_xlim((0, 10))
    ax1.set_ylim((-1.5, 1.5))
    ax1.set_xlabel(r'Bit Location')
    ax1.set_ylabel(r'Bit Value')
    ax2.set_xlim((0, 10))
    ax2.set_ylim((-1.5, 1.5))
    ax2.set_ylabel(r'Confidence level')

    ax1.set_yticks([-1, 1, ], ['0', '1'])
    ax2.set_yticks([0, ], ['Threshold'], fontsize=5)

    l1, = ax1.plot(x, y1, color='r')
    l2, = ax2.plot(x, y2, color='b')

    annotation1_x = 2
    annotation1_y = math.sin(annotation1_x)
    plt.plot([annotation1_x, annotation1_x], [annotation1_y, -1.5], 'k--')
    plt.annotate('high confidence bit \'1\'', xy=(annotation1_x, annotation1_y), xycoords='data', xytext=(+30, -30),
                 textcoords='offset points', arrowprops=dict(arrowstyle='->', connectionstyle='arc3, rad=.2'))

    annotation2_x = 6
    annotation2_y = math.sin(annotation2_x)
    plt.plot([annotation2_x, annotation2_x], [annotation2_y, -1.5], 'k--')
    plt.annotate('low confidence bit \'0\'', xy=(annotation2_x, annotation2_y), xycoords='data', xytext=(+30, -30),
                 textcoords='offset points', arrowprops=dict(arrowstyle='->', connectionstyle='arc3, rad=.2'))

    plt.title(r'SOFT', fontsize=20, weight='bold')

    plt.savefig("bin/soft.pdf", format="pdf")  # latex

    plt.show()

    return
