import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter

from src.tool.dataset import initNonSNRStack, loadEDFlag, loadFairIndex, MidDataset
from src.tool.formatnum import formatnum


def drawFairness():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize datasets
    NonSNRStackDataset = initNonSNRStack()

    # Load datasets
    ED = 0
    Flag = loadEDFlag(NonSNRStackDataset[MidDataset])
    (x1, y1) = loadFairIndex(NonSNRStackDataset[MidDataset])

    # Extract data of ED
    Flaglocation = []
    for loopcount in range(len(Flag)):
        if Flag[loopcount] == ED:
            Flaglocation.append(loopcount)

    realx1 = []
    realy1 = []

    for i in range(0, len(x1)):
        for j in range(0, len(Flaglocation)):
            if i == Flaglocation[j]:
                realx1.append(x1[i])
    for i in range(0, len(y1)):
        for j in range(0, len(Flaglocation)):
            if i == Flaglocation[j]:
                realy1.append(y1[i])

    # Initialize subplots
    fig, ax1 = plt.subplots()

    # Initialize axis
    ax1.ticklabel_format(axis="both", style="sci", scilimits=(0, 0))
    ax1.set_xlabel(r'Time (ms)', fontsize=15)
    ax1.set_ylabel('Jain\'s Fairness Index', fontsize=15)

    # Draw lines
    ax1.plot(realx1, realy1, color='r')

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw gridlines
    ax1.grid()

    # Save subplots to files
    plt.savefig("bin/Fairness.pdf", format="pdf", transparent="ture")  # latex

    # Show subplots
    plt.show()

    return
