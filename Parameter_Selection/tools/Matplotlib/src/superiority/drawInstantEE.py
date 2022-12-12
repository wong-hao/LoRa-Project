import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter

from src.tool import dataset
from src.tool.dataset import initNonSNRStack, initDyLoRa, RealEDNum, loadEDjFinalInstantEE
from src.tool.formatnum import formatnum


def drawInstantEEED2():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Init datasets
    x = np.linspace(dataset.GWNumMin, dataset.GWNumMax, dataset.GWNum)

    NonSNRStackDataset = initNonSNRStack()
    DyLoRaDataset = initDyLoRa()

    # Load datasets
    y1 = []
    y2 = []

    for loopcount in range(dataset.GWNum * dataset.EDLevel):
        InstantEE = []
        DyLoRaInstantEE = []

        for loopcount2 in range(RealEDNum):
            InstantEE.append(loadEDjFinalInstantEE(loopcount2, NonSNRStackDataset[loopcount]))
            DyLoRaInstantEE.append(loadEDjFinalInstantEE(loopcount2, DyLoRaDataset[loopcount]))

        if loopcount % dataset.EDLevel == 0:
            y1.append(np.mean(InstantEE))
            y2.append(np.mean(DyLoRaInstantEE))

    print(y1)
    print(y2)


    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Draw two lines
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
                  label='EELoRa')
    ax1.plot(x, y2, color='b', marker='x', markersize=7, markeredgecolor='b',
                  label='DyLoRa')

    # Initialize axis
    ax1.set_xlabel(r'Number of gateways', fontsize=15)
    ax1.set_ylabel(r'Energy Efficiency (bit/mj)', fontsize=15)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw legends
    plt.legend(loc='best',
               fontsize=14,
               ncol=3)

    # Draw gridlines
    ax1.grid()

    # Draw title
    # plt.title(r'Instant Packet Deliver Ratio')

    # Save subplots to files
    plt.savefig("bin/InstantEE(GWNum)(ED=2).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()
