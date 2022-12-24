import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter

from src.tool import dataset
from src.tool.dataset import initNonSNRStack, initEFLoRa, loadEDjFinalInstantEE
from src.tool.formatnum import formatnum

RealEDNum = 2



def drawInstantEEED2():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Init datasets
    x = np.linspace(dataset.GWNumMin, dataset.GWNumMax, 3)

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1 = []
    y2 = []

    for loopcount in range(dataset.GWNum * dataset.EDLevel):
        InstantEE = []
        EFLoRaInstantEE = []

        for loopcount2 in range(RealEDNum):
            InstantEE.append(loadEDjFinalInstantEE(loopcount2, NonSNRStackDataset[loopcount]))
            EFLoRaInstantEE.append(loadEDjFinalInstantEE(loopcount2, EFLoRaDataset[loopcount]))

        if loopcount % (2*dataset.EDLevel) == 0:
            y1.append(np.mean(InstantEE))
            y2.append(np.mean(EFLoRaInstantEE))

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
    plt.savefig("bin/InstantEE(GWNum)(ED=2).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

def drawInstantEEED4():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Init datasets
    x = np.linspace(dataset.GWNumMin, dataset.GWNumMax, 3)

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1 = []
    y2 = []

    for loopcount in range(dataset.GWNum * dataset.EDLevel):
        InstantEE = []
        EFLoRaInstantEE = []

        for loopcount2 in range(RealEDNum):
            InstantEE.append(loadEDjFinalInstantEE(loopcount2, NonSNRStackDataset[loopcount]))
            EFLoRaInstantEE.append(loadEDjFinalInstantEE(loopcount2, EFLoRaDataset[loopcount]))

        if loopcount % (2*dataset.EDLevel) == 1:
            y1.append(np.mean(InstantEE))
            y2.append(np.mean(EFLoRaInstantEE))

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
    plt.savefig("bin/InstantEE(GWNum)(ED=4).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

def drawInstantEEED6():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Init datasets
    x = np.linspace(dataset.GWNumMin, dataset.GWNumMax, 3)

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1 = []
    y2 = []

    for loopcount in range(dataset.GWNum * dataset.EDLevel):
        InstantEE = []
        EFLoRaInstantEE = []

        for loopcount2 in range(RealEDNum):
            InstantEE.append(loadEDjFinalInstantEE(loopcount2, NonSNRStackDataset[loopcount]))
            EFLoRaInstantEE.append(loadEDjFinalInstantEE(loopcount2, EFLoRaDataset[loopcount]))

        if loopcount % (2*dataset.EDLevel) == 2:
            y1.append(np.mean(InstantEE))
            y2.append(np.mean(EFLoRaInstantEE))

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
    plt.savefig("bin/InstantEE(GWNum)(ED=6).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

def drawInstantEEED8():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Init datasets
    x = np.linspace(dataset.GWNumMin, dataset.GWNumMax, 3)

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1 = []
    y2 = []

    for loopcount in range(dataset.GWNum * dataset.EDLevel):
        InstantEE = []
        EFLoRaInstantEE = []

        for loopcount2 in range(RealEDNum):
            InstantEE.append(loadEDjFinalInstantEE(loopcount2, NonSNRStackDataset[loopcount]))
            EFLoRaInstantEE.append(loadEDjFinalInstantEE(loopcount2, EFLoRaDataset[loopcount]))

        if loopcount % (2*dataset.EDLevel) == 3:
            y1.append(np.mean(InstantEE))
            y2.append(np.mean(EFLoRaInstantEE))

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
    plt.savefig("bin/InstantEE(GWNum)(ED=8).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

def drawInstantEEGW2():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Init datasets
    x = np.linspace(dataset.EDNumMin, dataset.EDNumMax, dataset.EDLevel)

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1 = []
    y2 = []

    for loopcount in range(dataset.GWNum * dataset.EDLevel):
        InstantEE = []
        EFLoRaInstantEE = []

        for loopcount2 in range(RealEDNum):
            InstantEE.append(loadEDjFinalInstantEE(loopcount2, NonSNRStackDataset[loopcount]))
            EFLoRaInstantEE.append(loadEDjFinalInstantEE(loopcount2, EFLoRaDataset[loopcount]))

        if 8*0 <= loopcount <= 8*0+3:
            y1.append(np.mean(InstantEE))
            y2.append(np.mean(EFLoRaInstantEE))

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
    plt.savefig("bin/InstantEE(EDNum)(GW=2).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

def drawInstantEEGW4():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Init datasets
    x = np.linspace(dataset.EDNumMin, dataset.EDNumMax, dataset.EDLevel)

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1 = []
    y2 = []

    for loopcount in range(dataset.GWNum * dataset.EDLevel):
        InstantEE = []
        EFLoRaInstantEE = []

        for loopcount2 in range(RealEDNum):
            InstantEE.append(loadEDjFinalInstantEE(loopcount2, NonSNRStackDataset[loopcount]))
            EFLoRaInstantEE.append(loadEDjFinalInstantEE(loopcount2, EFLoRaDataset[loopcount]))

        if 8*1 <= loopcount <= 8*1+3:
            y1.append(np.mean(InstantEE))
            y2.append(np.mean(EFLoRaInstantEE))

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
    plt.savefig("bin/InstantEE(EDNum)(GW=4).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

def drawInstantEEGW6():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Init datasets
    x = np.linspace(dataset.EDNumMin, dataset.EDNumMax, dataset.EDLevel)

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1 = []
    y2 = []

    for loopcount in range(dataset.GWNum * dataset.EDLevel):
        InstantEE = []
        EFLoRaInstantEE = []

        for loopcount2 in range(RealEDNum):
            InstantEE.append(loadEDjFinalInstantEE(loopcount2, NonSNRStackDataset[loopcount]))
            EFLoRaInstantEE.append(loadEDjFinalInstantEE(loopcount2, EFLoRaDataset[loopcount]))

        if 8*2 <= loopcount <= 8*2+3:
            y1.append(np.mean(InstantEE))
            y2.append(np.mean(EFLoRaInstantEE))

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
    plt.savefig("bin/InstantEE(EDNum)(GW=6).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()