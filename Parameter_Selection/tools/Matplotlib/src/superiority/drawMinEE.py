import math

import numpy as np
from matplotlib import pyplot as plt

from src.tool.Avg import average_triplets
from src.tool.dataset import initNonSNRStack, loadFinalMinEE, initEFLoRa, GWNum, EDLevel, GWNumMin, EDNumMin, GWNumMax, \
    EDNumMax, TotalDataset, n
from src.tool.formatfile import svg2emf


def drawMinEEED4():
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
        minEE = loadFinalMinEE(NonSNRStackDataset[loopcount])
        EFLoRaminEE = loadFinalMinEE(EFLoRaDataset[loopcount])

        if loopcount % (2 * EDLevel * n) == n:
            y1data[0].append(minEE)
            y2data[0].append(EFLoRaminEE)
        elif loopcount % (2 * EDLevel * n) == n+1:
            y1data[1].append(minEE)
            y2data[1].append(EFLoRaminEE)
        elif loopcount % (2 * EDLevel * n) == n+2:
            y1data[2].append(minEE)
            y2data[2].append(EFLoRaminEE)

    y1 = average_triplets(y1data)
    y2 = average_triplets(y2data)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Draw two lines
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='EEADR')
    ax1.plot(x, y2, color='b', marker='x', markersize=7, markeredgecolor='b',
             label='EF-LoRa')

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
    filename = "bin/MinEE(GWNum)(ED=4).svg"
    svg2emf(filename)

    # Show subplots
    plt.show()

def drawMinEEED8():
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
        minEE = loadFinalMinEE(NonSNRStackDataset[loopcount])
        EFLoRaminEE = loadFinalMinEE(EFLoRaDataset[loopcount])

        if loopcount % (2 * EDLevel * n) == 3*n:
            y1data[0].append(minEE)
            y2data[0].append(EFLoRaminEE)
        elif loopcount % (2 * EDLevel * n) == 3*n+1:
            y1data[1].append(minEE)
            y2data[1].append(EFLoRaminEE)
        elif loopcount % (2 * EDLevel * n) == 3*n+2:
            y1data[2].append(minEE)
            y2data[2].append(EFLoRaminEE)

    y1 = average_triplets(y1data)
    y2 = average_triplets(y2data)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Draw two lines
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='EEADR')
    ax1.plot(x, y2, color='b', marker='x', markersize=7, markeredgecolor='b',
             label='EF-LoRa')

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
    filename = "bin/MinEE(GWNum)(ED=8).svg"
    svg2emf(filename)

    # Show subplots
    plt.show()

def drawMinEEED12():
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
        minEE = loadFinalMinEE(NonSNRStackDataset[loopcount])
        EFLoRaminEE = loadFinalMinEE(EFLoRaDataset[loopcount])

        if loopcount % (2 * EDLevel * n) == 5*n:
            y1data[0].append(minEE)
            y2data[0].append(EFLoRaminEE)
        elif loopcount % (2 * EDLevel * n) == 5*n+1:
            y1data[1].append(minEE)
            y2data[1].append(EFLoRaminEE)
        elif loopcount % (2 * EDLevel * n) == 5*n+2:
            y1data[2].append(minEE)
            y2data[2].append(EFLoRaminEE)

    y1 = average_triplets(y1data)
    y2 = average_triplets(y2data)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Draw two lines
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='EEADR')
    ax1.plot(x, y2, color='b', marker='x', markersize=7, markeredgecolor='b',
             label='EF-LoRa')

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
    filename = "bin/MinEE(GWNum)(ED=12).svg"
    svg2emf(filename)

    # Show subplots
    plt.show()

def drawMinEEGW2():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize datasets
    x = np.linspace(EDNumMin, EDNumMax, EDLevel)

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1 = []
    y2 = []

    for loopcount in range(GWNum * EDLevel):
        minEE = loadFinalMinEE(NonSNRStackDataset[loopcount])
        EFLoRaminEE = loadFinalMinEE(EFLoRaDataset[loopcount])

        if (2 * EDLevel)*0 <= loopcount <= (2 * EDLevel)*0+5:
            y1.append(minEE)
            y2.append(EFLoRaminEE)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Draw two lines
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='EEADR')
    ax1.plot(x, y2, color='b', marker='x', markersize=7, markeredgecolor='b',
             label='EF-LoRa')

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
    filename = "bin/MinEE(EDNum)(GW=2).svg"
    svg2emf(filename)

    # Show subplots
    plt.show()

def drawMinEEGW4():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize datasets
    x = np.linspace(EDNumMin, EDNumMax, EDLevel)

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1 = []
    y2 = []

    for loopcount in range(GWNum * EDLevel):
        minEE = loadFinalMinEE(NonSNRStackDataset[loopcount])
        EFLoRaminEE = loadFinalMinEE(EFLoRaDataset[loopcount])

        if (2 * EDLevel)*1 <= loopcount <= (2 * EDLevel)*1+5:
            y1.append(minEE)
            y2.append(EFLoRaminEE)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Draw two lines
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='EEADR')
    ax1.plot(x, y2, color='b', marker='x', markersize=7, markeredgecolor='b',
             label='EF-LoRa')

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
    filename = "bin/MinEE(EDNum)(GW=4).svg"
    svg2emf(filename)

    # Show subplots
    plt.show()

def drawMinEEGW6():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize datasets
    x = np.linspace(EDNumMin, EDNumMax, EDLevel)

    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    y1 = []
    y2 = []

    for loopcount in range(GWNum * EDLevel):
        minEE = loadFinalMinEE(NonSNRStackDataset[loopcount])
        EFLoRaminEE = loadFinalMinEE(EFLoRaDataset[loopcount])

        if (2 * EDLevel) * 2 <= loopcount <= (2 * EDLevel) * 2 + 5:
            y1.append(minEE)
            y2.append(EFLoRaminEE)

    # Initialize subplots
    fig, ax1 = plt.subplots()

    # Draw two lines
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='EEADR')
    ax1.plot(x, y2, color='b', marker='x', markersize=7, markeredgecolor='b',
             label='EF-LoRa')

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
    filename = "bin/MinEE(EDNum)(GW=6).svg"
    svg2emf(filename)

    # Show subplots
    plt.show()
