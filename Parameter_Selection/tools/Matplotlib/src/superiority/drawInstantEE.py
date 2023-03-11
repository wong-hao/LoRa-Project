import math

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter

from src.tool import dataset
from src.tool.dataset import initNonSNRStack, initEFLoRa, loadEDjFinalInstantEE, GWNum, EDLevel, GWNumMin, GWNumMax, \
    EDNumMin, EDNumMax, TotalDataset, n
from src.tool.formatnum import formatnum


def drawInstantEEED4():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize datasets
    x = np.linspace(GWNumMin, GWNumMax, math.ceil(GWNum/2))

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1data = [[], [], []]
    y2data = [[], [], []]

    for loopcount in range(TotalDataset):
        InstantEE = []
        EFLoRaInstantEE = []

        for loopcount2 in range(2 * EDLevel):
            # 求最后一一行在所有节点中所有非零EE值的平均值
            if loadEDjFinalInstantEE(loopcount2, NonSNRStackDataset[loopcount]) != 0 and loadEDjFinalInstantEE(loopcount2, EFLoRaDataset[loopcount]) != 0 :
                InstantEE.append(loadEDjFinalInstantEE(loopcount2, NonSNRStackDataset[loopcount]))
                EFLoRaInstantEE.append(loadEDjFinalInstantEE(loopcount2, EFLoRaDataset[loopcount]))

        if loopcount % (2 * EDLevel * n) == n:
            y1data[0].append(np.mean(InstantEE))
            y2data[0].append(np.mean(EFLoRaInstantEE))
        elif loopcount % (2 * EDLevel * n) == n+1:
            y1data[1].append(np.mean(InstantEE))
            y2data[1].append(np.mean(EFLoRaInstantEE))
        elif loopcount % (2 * EDLevel * n) == n+2:
            y1data[2].append(np.mean(InstantEE))
            y2data[2].append(np.mean(EFLoRaInstantEE))

    Zippedy1 = zip(y1data[0], y1data[1], y1data[2])
    Zippedy2 = zip(y2data[0], y2data[1], y2data[2])

    y1 = [(a + b + c) / len(y1data) for a, b, c in Zippedy1]
    y2 = [(a + b + c) / len(y2data) for a, b, c in Zippedy2]

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Draw two lines
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='EELoRa')
    ax1.plot(x, y2, color='b', marker='x', markersize=7, markeredgecolor='b',
             label='EFLoRa')

    # Initialize axis
    ax1.set_xlabel(r'Number of gateways', fontsize=15)
    ax1.set_ylabel(r'Energy Efficiency (bit/mJ)', fontsize=15)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw legends
    plt.legend(loc='best',
               fontsize=14,
               ncol=3)

    # Draw gridlines
    ax1.grid()

    # Save subplots to files
    plt.savefig("bin/InstantEE(GWNum)(ED=4).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()


def drawInstantEEED8():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize datasets
    x = np.linspace(GWNumMin, GWNumMax, math.ceil(GWNum/2))

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1data = [[], [], []]
    y2data = [[], [], []]

    for loopcount in range(TotalDataset):
        InstantEE = []
        EFLoRaInstantEE = []

        for loopcount2 in range(2 * EDLevel):
            # 求最后一一行在所有节点中所有非零EE值的平均值
            if loadEDjFinalInstantEE(loopcount2, NonSNRStackDataset[loopcount]) != 0 and loadEDjFinalInstantEE(loopcount2, EFLoRaDataset[loopcount]) != 0 :
                InstantEE.append(loadEDjFinalInstantEE(loopcount2, NonSNRStackDataset[loopcount]))
                EFLoRaInstantEE.append(loadEDjFinalInstantEE(loopcount2, EFLoRaDataset[loopcount]))

        if loopcount % (2 * EDLevel * n) == 3*n:
            y1data[0].append(np.mean(InstantEE))
            y2data[0].append(np.mean(EFLoRaInstantEE))
        elif loopcount % (2 * EDLevel * n) == 3*n+1:
            y1data[1].append(np.mean(InstantEE))
            y2data[1].append(np.mean(EFLoRaInstantEE))
        elif loopcount % (2 * EDLevel * n) == 3*n+2:
            y1data[2].append(np.mean(InstantEE))
            y2data[2].append(np.mean(EFLoRaInstantEE))

    Zippedy1 = zip(y1data[0], y1data[1], y1data[2])
    Zippedy2 = zip(y2data[0], y2data[1], y2data[2])

    y1 = [(a + b + c) / len(y1data) for a, b, c in Zippedy1]
    y2 = [(a + b + c) / len(y2data) for a, b, c in Zippedy2]

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Draw two lines
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='EELoRa')
    ax1.plot(x, y2, color='b', marker='x', markersize=7, markeredgecolor='b',
             label='EFLoRa')

    # Initialize axis
    ax1.set_xlabel(r'Number of gateways', fontsize=15)
    ax1.set_ylabel(r'Energy Efficiency (bit/mJ)', fontsize=15)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw legends
    plt.legend(loc='best',
               fontsize=14,
               ncol=3)

    # Draw gridlines
    ax1.grid()

    # Save subplots to files
    plt.savefig("bin/InstantEE(GWNum)(ED=8).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()


def drawInstantEEED12():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize datasets
    x = np.linspace(GWNumMin, GWNumMax, math.ceil(GWNum/2))

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1data = [[], [], []]
    y2data = [[], [], []]

    for loopcount in range(TotalDataset):
        InstantEE = []
        EFLoRaInstantEE = []

        for loopcount2 in range(2 * EDLevel):
            # 求最后一一行在所有节点中所有非零EE值的平均值
            if loadEDjFinalInstantEE(loopcount2, NonSNRStackDataset[loopcount]) != 0 and loadEDjFinalInstantEE(loopcount2, EFLoRaDataset[loopcount]) != 0 :
                InstantEE.append(loadEDjFinalInstantEE(loopcount2, NonSNRStackDataset[loopcount]))
                EFLoRaInstantEE.append(loadEDjFinalInstantEE(loopcount2, EFLoRaDataset[loopcount]))

        if loopcount % (2 * EDLevel * n) == 5*n:
            y1data[0].append(np.mean(InstantEE))
            y2data[0].append(np.mean(EFLoRaInstantEE))
        elif loopcount % (2 * EDLevel * n) == 5*n+1:
            y1data[1].append(np.mean(InstantEE))
            y2data[1].append(np.mean(EFLoRaInstantEE))
        elif loopcount % (2 * EDLevel * n) == 5*n+2:
            y1data[2].append(np.mean(InstantEE))
            y2data[2].append(np.mean(EFLoRaInstantEE))

    Zippedy1 = zip(y1data[0], y1data[1], y1data[2])
    Zippedy2 = zip(y2data[0], y2data[1], y2data[2])

    y1 = [(a + b + c) / len(y1data) for a, b, c in Zippedy1]
    y2 = [(a + b + c) / len(y2data) for a, b, c in Zippedy2]

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Draw two lines
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='EELoRa')
    ax1.plot(x, y2, color='b', marker='x', markersize=7, markeredgecolor='b',
             label='EFLoRa')

    # Initialize axis
    ax1.set_xlabel(r'Number of gateways', fontsize=15)
    ax1.set_ylabel(r'Energy Efficiency (bit/mJ)', fontsize=15)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw legends
    plt.legend(loc='best',
               fontsize=14,
               ncol=3)

    # Draw gridlines
    ax1.grid()

    # Save subplots to files
    plt.savefig("bin/InstantEE(GWNum)(ED=12).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

def drawInstantEEGW2():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize datasets
    x = np.linspace(EDNumMin, EDNumMax, EDLevel)

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1 = []
    y2 = []

    for loopcount in range(TotalDataset):
        InstantEE = []
        EFLoRaInstantEE = []

        for loopcount2 in range(2 * EDLevel):
            if loadEDjFinalInstantEE(loopcount2, NonSNRStackDataset[loopcount]) != 0 and loadEDjFinalInstantEE(loopcount2, EFLoRaDataset[loopcount]) != 0 :
                InstantEE.append(loadEDjFinalInstantEE(loopcount2, NonSNRStackDataset[loopcount]))
                EFLoRaInstantEE.append(loadEDjFinalInstantEE(loopcount2, EFLoRaDataset[loopcount]))

        if (2 * EDLevel) * 0 <= loopcount <= (2 * EDLevel) * 0 + 5:
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
    ax1.set_ylabel(r'Energy Efficiency (bit/mJ)', fontsize=15)

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

    # Initialize datasets
    x = np.linspace(EDNumMin, EDNumMax, EDLevel)

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1 = []
    y2 = []

    for loopcount in range(TotalDataset):
        InstantEE = []
        EFLoRaInstantEE = []

        for loopcount2 in range(2 * EDLevel):
            if loadEDjFinalInstantEE(loopcount2, NonSNRStackDataset[loopcount]) != 0 and loadEDjFinalInstantEE(loopcount2, EFLoRaDataset[loopcount]) != 0 :
                InstantEE.append(loadEDjFinalInstantEE(loopcount2, NonSNRStackDataset[loopcount]))
                EFLoRaInstantEE.append(loadEDjFinalInstantEE(loopcount2, EFLoRaDataset[loopcount]))

        if (2 * EDLevel) * 1 <= loopcount <= (2 * EDLevel) * 1 + 5:
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
    ax1.set_ylabel(r'Energy Efficiency (bit/mJ)', fontsize=15)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw legends
    plt.legend(loc='best',
               fontsize=14,
               ncol=3)

    # Draw gridlines
    ax1.grid()

    # Save subplots to files
    plt.savefig("bin/InstantEE(EDNum)(GW=4).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()


def drawInstantEEGW6():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize datasets
    x = np.linspace(EDNumMin, EDNumMax, EDLevel)

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1 = []
    y2 = []

    for loopcount in range(TotalDataset):
        InstantEE = []
        EFLoRaInstantEE = []

        for loopcount2 in range(2 * EDLevel):
            if loadEDjFinalInstantEE(loopcount2, NonSNRStackDataset[loopcount]) != 0 and loadEDjFinalInstantEE(loopcount2, EFLoRaDataset[loopcount]) != 0 :
                InstantEE.append(loadEDjFinalInstantEE(loopcount2, NonSNRStackDataset[loopcount]))
                EFLoRaInstantEE.append(loadEDjFinalInstantEE(loopcount2, EFLoRaDataset[loopcount]))

        if (2 * EDLevel) * 2 <= loopcount <= (2 * EDLevel) * 2 + 5:
            y1.append(np.mean(InstantEE))
            y2.append(np.mean(EFLoRaInstantEE))

    # Initialize subplots
    fig, ax1 = plt.subplots()

    # Draw two lines
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='EELoRa')
    ax1.plot(x, y2, color='b', marker='x', markersize=7, markeredgecolor='b',
             label='EFLoRa')

    # Initialize axis
    ax1.set_xlabel(r'Number of devices', fontsize=15)
    ax1.set_ylabel(r'Energy Efficiency (bit/mJ)', fontsize=15)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw legends
    plt.legend(loc='best',
               fontsize=14,
               ncol=3)

    # Draw gridlines
    ax1.grid()

    # Save subplots to files
    plt.savefig("bin/InstantEE(EDNum)(GW=6).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()
