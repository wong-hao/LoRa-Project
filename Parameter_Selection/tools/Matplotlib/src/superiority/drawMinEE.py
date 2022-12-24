import numpy as np
from matplotlib import pyplot as plt

from src.tool import dataset
from src.tool.dataset import initNonSNRStack, loadFinalMinEE, initEFLoRa, GWNum, EDLevel, GWNumMin, EDNumMin, GWNumMax, \
    EDNumMax


def drawMinEEED2():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Init datasets
    x = np.linspace(GWNumMin, GWNumMax, 3)

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1 = []
    y2 = []

    for loopcount in range(GWNum * EDLevel):
        minEE = loadFinalMinEE(NonSNRStackDataset[loopcount])
        EFLoRaminEE = loadFinalMinEE(EFLoRaDataset[loopcount])

        if loopcount % (2 * EDLevel) == 0:
            y1.append(minEE)
            y2.append(EFLoRaminEE)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Draw two lines
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='EELoRa')
    ax1.plot(x, y2, color='b', marker='x', markersize=7, markeredgecolor='b',
             label='EFLoRa')

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
    plt.savefig("bin/MinEE(GWNum)(ED=2).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

def drawMinEEED4():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Init datasets
    x = np.linspace(GWNumMin, GWNumMax, 3)

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1 = []
    y2 = []

    for loopcount in range(GWNum * EDLevel):
        minEE = loadFinalMinEE(NonSNRStackDataset[loopcount])
        EFLoRaminEE = loadFinalMinEE(EFLoRaDataset[loopcount])

        if loopcount % (2 * EDLevel) == 1:
            y1.append(minEE)
            y2.append(EFLoRaminEE)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Draw two lines
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='EELoRa')
    ax1.plot(x, y2, color='b', marker='x', markersize=7, markeredgecolor='b',
             label='EFLoRa')

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
    plt.savefig("bin/MinEE(GWNum)(ED=4).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

def drawMinEEED6():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Init datasets
    x = np.linspace(GWNumMin, GWNumMax, 3)

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1 = []
    y2 = []

    for loopcount in range(GWNum * EDLevel):
        minEE = loadFinalMinEE(NonSNRStackDataset[loopcount])
        EFLoRaminEE = loadFinalMinEE(EFLoRaDataset[loopcount])

        if loopcount % (2 * EDLevel) == 2:
            y1.append(minEE)
            y2.append(EFLoRaminEE)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Draw two lines
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='EELoRa')
    ax1.plot(x, y2, color='b', marker='x', markersize=7, markeredgecolor='b',
             label='EFLoRa')

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
    plt.savefig("bin/MinEE(GWNum)(ED=6).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

def drawMinEEED8():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Init datasets
    x = np.linspace(GWNumMin, GWNumMax, 3)

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1 = []
    y2 = []

    for loopcount in range(GWNum * EDLevel):
        minEE = loadFinalMinEE(NonSNRStackDataset[loopcount])
        EFLoRaminEE = loadFinalMinEE(EFLoRaDataset[loopcount])

        if loopcount % (2 * EDLevel) == 3:
            y1.append(minEE)
            y2.append(EFLoRaminEE)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Draw two lines
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='EELoRa')
    ax1.plot(x, y2, color='b', marker='x', markersize=7, markeredgecolor='b',
             label='EFLoRa')

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
    plt.savefig("bin/MinEE(GWNum)(ED=8).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

def drawMinEEGW2():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Init datasets
    x = np.linspace(EDNumMin, EDNumMax, EDLevel)

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1 = []
    y2 = []

    for loopcount in range(GWNum * EDLevel):
        minEE = loadFinalMinEE(NonSNRStackDataset[loopcount])
        EFLoRaminEE = loadFinalMinEE(EFLoRaDataset[loopcount])

        if 8*0 <= loopcount <= 8*0+3:
            y1.append(minEE)
            y2.append(EFLoRaminEE)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Draw two lines
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='EELoRa')
    ax1.plot(x, y2, color='b', marker='x', markersize=7, markeredgecolor='b',
             label='EFLoRa')

    # Initialize axis
    ax1.set_xlabel(r'Number of devices', fontsize=15)
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
    plt.savefig("bin/MinEE(EDNum)(GW=2).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

def drawMinEEGW4():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Init datasets
    x = np.linspace(EDNumMin, EDNumMax, EDLevel)

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1 = []
    y2 = []

    for loopcount in range(GWNum * EDLevel):
        minEE = loadFinalMinEE(NonSNRStackDataset[loopcount])
        EFLoRaminEE = loadFinalMinEE(EFLoRaDataset[loopcount])

        if 8*1 <= loopcount <= 8*1+3:
            y1.append(minEE)
            y2.append(EFLoRaminEE)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Draw two lines
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='EELoRa')
    ax1.plot(x, y2, color='b', marker='x', markersize=7, markeredgecolor='b',
             label='EFLoRa')

    # Initialize axis
    ax1.set_xlabel(r'Number of devices', fontsize=15)
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
    plt.savefig("bin/MinEE(EDNum)(GW=4).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

def drawMinEEGW6():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Init datasets
    x = np.linspace(EDNumMin, EDNumMax, EDLevel)

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1 = []
    y2 = []

    for loopcount in range(GWNum * EDLevel):
        minEE = loadFinalMinEE(NonSNRStackDataset[loopcount])
        EFLoRaminEE = loadFinalMinEE(EFLoRaDataset[loopcount])

        if 8 * 2 <= loopcount <= 8 * 2 + 3:
            y1.append(minEE)
            y2.append(EFLoRaminEE)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Draw two lines
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='EELoRa')
    ax1.plot(x, y2, color='b', marker='x', markersize=7, markeredgecolor='b',
             label='EFLoRa')

    # Initialize axis
    ax1.set_xlabel(r'Number of devices', fontsize=15)
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
    plt.savefig("bin/MinEE(EDNum)(GW=6).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()
