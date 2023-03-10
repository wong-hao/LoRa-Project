import matplotlib.pyplot as plt
import numpy as np

from src.tool import Dataset
from src.tool.Avg import getAvgNum
from src.tool.Dataset import initCSNonPara, loadCSPDR, initCSOPRNonPara, GWNumMin, GWNumMax, GWNum, JXLevel, \
    TotalDataset, loadCSFinalPDR, n


def drawSupCSPDRJX2():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize datasets
    x = np.linspace(GWNumMin, GWNumMax, GWNum)

    CSNonParaDataset = initCSNonPara()
    CSOPRNonParaDataset = initCSOPRNonPara()

    # Load datasets
    GW0data0 = []
    GW0data1 = []
    GW0data2 = []
    OPRGW0data0 = []
    OPRGW0data1 = []
    OPRGW0data2 = []

    for loopcount in range(TotalDataset):

        if loopcount % (n * JXLevel) == 0:
            GW0data0.append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGW0data0.append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))
        elif loopcount % (n * JXLevel) == 1:
            GW0data1.append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGW0data1.append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))
        elif loopcount % (n * JXLevel) == 2:
            GW0data2.append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGW0data2.append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))

    ZippedGW0 = zip(GW0data0, GW0data1, GW0data1)
    ZippedOPRGW0 = zip(OPRGW0data0, OPRGW0data1, OPRGW0data2)

    GW0 = [(a + b + c) / n for a, b, c in ZippedGW0]
    OPRGW0 = [(a + b + c) / n for a, b, c in ZippedOPRGW0]

    # Initialize subplots
    fig, ax1 = plt.subplots()

    # Initialize subplot1 yxis
    ax1.set_ylim((0, 1))

    '''
    # Draw two error charts
    yerr = [0, 0, 0, 0, 0]
    ax1.errorbar(x, GW0, yerr=yerr, ecolor='black', color='b', capsize=4, marker='x', markersize=7, markeredgecolor='b',
                 label='ReLoRaWAN')
    ax1.errorbar(x, OPRGW0, yerr=yerr, ecolor='black', color='r', capsize=4, marker='o', markersize=7,
                 markeredgecolor='r', label='OPR')
    '''

    # Draw two lines
    ax1.plot(x, GW0, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='ReLoRaWAN')
    ax1.plot(x, OPRGW0, color='b', marker='o', markersize=7, markeredgecolor='b',
             label='OPR', linestyle='dashed')

    # Draw gridlines
    ax1.grid()

    # Draw legends
    plt.legend(fontsize=15,
               loc='lower left')
    # Draw title
    # plt.title(r'Instant Packet Delivery Ratio')

    # Initialize axis
    ax1.set_xlabel(r'Number of gateways', fontsize=15)
    ax1.set_ylabel(r'Packet Delivery Ratio', fontsize=15)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Save subplots to files
    plt.savefig("bin/PDR(GWNum)(JX=2).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return


def drawSupCSPDRJX4():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize datasets
    x = np.linspace(GWNumMin, GWNumMax, GWNum)

    CSNonParaDataset = initCSNonPara()
    CSOPRNonParaDataset = initCSOPRNonPara()

    # Load datasets
    GW1data0 = []
    GW1data1 = []
    GW1data2 = []
    OPRGW1data0 = []
    OPRGW1data1 = []
    OPRGW1data2 = []

    for loopcount in range(TotalDataset):

        if loopcount % (n * JXLevel) == 3:
            GW1data0.append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGW1data0.append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))
        elif loopcount % (n * JXLevel) == 4:
            GW1data1.append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGW1data1.append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))
        elif loopcount % (n * JXLevel) == 5:
            GW1data2.append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGW1data2.append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))

    ZippedGW1 = zip(GW1data0, GW1data1, GW1data2)
    ZippedOPRGW1 = zip(OPRGW1data0, OPRGW1data1, OPRGW1data2)

    GW1 = [(a + b + c) / n for a, b, c in ZippedGW1]
    OPRGW1 = [(a + b + c) / n for a, b, c in ZippedOPRGW1]

    # Initialize subplots
    fig, ax1 = plt.subplots()

    # Initialize subplot1 yxis
    ax1.set_ylim((0, 1))

    # Draw two lines
    ax1.plot(x, GW1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='ReLoRaWAN')
    ax1.plot(x, OPRGW1, color='b', marker='o', markersize=7, markeredgecolor='b',
             label='OPR', linestyle='dashed')

    # Draw gridlines
    ax1.grid()

    # Draw legends
    plt.legend(fontsize=15,
               loc='lower left')

    # Draw title
    # plt.title(r'Instant Packet Deliver Ratio')

    # Initialize axis
    ax1.set_xlabel(r'Number of gateways', fontsize=15)
    ax1.set_ylabel(r'Packet Delivery Ratio', fontsize=15)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Save subplots to files
    plt.savefig("bin/PDR(GWNum)(JX=4).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return


def drawSupCSPDRJX6():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize datasets
    x = np.linspace(GWNumMin, GWNumMax, GWNum)

    CSNonParaDataset = initCSNonPara()
    CSOPRNonParaDataset = initCSOPRNonPara()

    # Load datasets
    GW2data0 = []
    GW2data1 = []
    GW2data2 = []
    OPRGW2data0 = []
    OPRGW2data1 = []
    OPRGW2data2 = []

    for loopcount in range(TotalDataset):

        if loopcount % (n * JXLevel) == 6:
            GW2data0.append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGW2data0.append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))
        elif loopcount % (n * JXLevel) == 7:
            GW2data1.append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGW2data1.append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))
        elif loopcount % (n * JXLevel) == 8:
            GW2data2.append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGW2data2.append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))

    ZippedGW2 = zip(GW2data0, GW2data1, GW2data2)
    ZippedOPRGW2 = zip(OPRGW2data0, OPRGW2data1, OPRGW2data2)

    GW2 = [(a + b + c) / n for a, b, c in ZippedGW2]
    OPRGW2 = [(a + b + c) / n for a, b, c in ZippedOPRGW2]

    # Initialize subplots
    fig, ax1 = plt.subplots()

    # Initialize subplot1 yxis
    ax1.set_ylim((0, 1))

    # Draw two lines
    ax1.plot(x, GW2, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='ReLoRaWAN')
    ax1.plot(x, OPRGW2, color='b', marker='o', markersize=7, markeredgecolor='b',
             label='OPR', linestyle='dashed')

    # Draw gridlines
    ax1.grid()

    # Draw legends
    plt.legend(fontsize=15,
               loc='lower left')

    # Draw title
    # plt.title(r'Instant Packet Deliver Ratio')

    # Initialize axis
    ax1.set_xlabel(r'Number of gateways', fontsize=15)
    ax1.set_ylabel(r'Packet Delivery Ratio', fontsize=15)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Save subplots to files
    plt.savefig("bin/PDR(GWNum)(JX=6).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return


def drawSupCSPDRJX8():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize datasets
    x = np.linspace(GWNumMin, GWNumMax, GWNum)

    CSNonParaDataset = initCSNonPara()
    CSOPRNonParaDataset = initCSOPRNonPara()

    # Load datasets
    GW3data0 = []
    GW3data1 = []
    GW3data2 = []
    OPRGW3data0 = []
    OPRGW3data1 = []
    OPRGW3data2 = []

    for loopcount in range(TotalDataset):

        if loopcount % (n * JXLevel) == 9:
            GW3data0.append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGW3data0.append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))
        elif loopcount % (n * JXLevel) == 10:
            GW3data1.append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGW3data1.append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))
        elif loopcount % (n * JXLevel) == 11:
            GW3data2.append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGW3data2.append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))

    ZippedGW3 = zip(GW3data0, GW3data1, GW3data2)
    ZippedOPRGW3 = zip(OPRGW3data0, OPRGW3data1, OPRGW3data2)

    GW3 = [(a + b + c) / n for a, b, c in ZippedGW3]
    OPRGW3 = [(a + b + c) / n for a, b, c in ZippedOPRGW3]

    # Initialize subplots
    fig, ax1 = plt.subplots()

    # Initialize subplot1 yxis
    ax1.set_ylim((0, 1))

    # Draw two lines
    ax1.plot(x, GW3, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='ReLoRaWAN')
    ax1.plot(x, OPRGW3, color='b', marker='o', markersize=7, markeredgecolor='b',
             label='OPR', linestyle='dashed')

    # Draw gridlines
    ax1.grid()

    # Draw legends
    plt.legend(fontsize=15,
               loc='lower left')

    # Draw title
    # plt.title(r'Instant Packet Deliver Ratio')

    # Initialize axis
    ax1.set_xlabel(r'Number of gateways', fontsize=15)
    ax1.set_ylabel(r'Packet Delivery Ratio', fontsize=15)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Save subplots to files
    plt.savefig("bin/PDR(GWNum)(JX=8).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return
