import matplotlib.pyplot as plt
import numpy as np

from src.tool import Dataset
from src.tool.Avg import getAvgNum, average_triplets
from src.tool.Dataset import initCSNonPara, loadCSPDR, initCSOPRNonPara, GWNumMin, GWNumMax, GWNum, JXLevel, \
    TotalDataset, loadCSFinalPDR, n
from src.tool.formatfile import svg2emf


def drawSupCSPDRJX2():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize datasets
    x = np.linspace(GWNumMin, GWNumMax, GWNum)

    CSNonParaDataset = initCSNonPara()
    CSOPRNonParaDataset = initCSOPRNonPara()

    # Load datasets
    GWdata  = [[], [], []]
    OPRGWdata = [[], [], []]

    for loopcount in range(TotalDataset):

        if loopcount % (n * JXLevel) == 0:
            GWdata[0].append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGWdata[0].append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))
        elif loopcount % (n * JXLevel) == 1:
            GWdata[1].append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGWdata[1].append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))
        elif loopcount % (n * JXLevel) == 2:
            GWdata[2].append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGWdata[2].append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))

    y1 = average_triplets(GWdata)
    y2 = average_triplets(OPRGWdata)

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
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='ReLoRaWAN')
    ax1.plot(x, y2, color='b', marker='o', markersize=7, markeredgecolor='b',
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
    filename = "bin/PDR(GWNum)(JX=2).svg"
    svg2emf(filename)

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
    GWdata  = [[], [], []]
    OPRGWdata = [[], [], []]

    for loopcount in range(TotalDataset):

        if loopcount % (n * JXLevel) == n:
            GWdata[0].append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGWdata[0].append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))
        elif loopcount % (n * JXLevel) == n+1:
            GWdata[1].append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGWdata[1].append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))
        elif loopcount % (n * JXLevel) == n+2:
            GWdata[2].append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGWdata[2].append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))

    ZippedGW = zip(GWdata[0], GWdata[1], GWdata[2])
    ZippedOPRGW = zip(OPRGWdata[0], OPRGWdata[1], OPRGWdata[2])

    y1 = [(a + b + c) / len(GWdata) for a, b, c in ZippedGW]
    y2 = [(a + b + c) / len(OPRGWdata) for a, b, c in ZippedOPRGW]

    # Initialize subplots
    fig, ax1 = plt.subplots()

    # Initialize subplot1 yxis
    ax1.set_ylim((0, 1))

    # Draw two lines
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='ReLoRaWAN')
    ax1.plot(x, y2, color='b', marker='o', markersize=7, markeredgecolor='b',
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
    filename = "bin/PDR(GWNum)(JX=4).svg"
    svg2emf(filename)

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
    GWdata  = [[], [], []]
    OPRGWdata = [[], [], []]

    for loopcount in range(TotalDataset):

        if loopcount % (n * JXLevel) == 2*n:
            GWdata[0].append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGWdata[0].append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))
        elif loopcount % (n * JXLevel) == 2*n+1:
            GWdata[1].append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGWdata[1].append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))
        elif loopcount % (n * JXLevel) == 2*n+2:
            GWdata[2].append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGWdata[2].append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))

    ZippedGW = zip(GWdata[0], GWdata[1], GWdata[2])
    ZippedOPRGW = zip(OPRGWdata[0], OPRGWdata[1], OPRGWdata[2])

    y1 = [(a + b + c) / len(GWdata) for a, b, c in ZippedGW]
    y2 = [(a + b + c) / len(OPRGWdata) for a, b, c in ZippedOPRGW]

    # Initialize subplots
    fig, ax1 = plt.subplots()

    # Initialize subplot1 yxis
    ax1.set_ylim((0, 1))

    # Draw two lines
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='ReLoRaWAN')
    ax1.plot(x, y2, color='b', marker='o', markersize=7, markeredgecolor='b',
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
    filename = "bin/PDR(GWNum)(JX=6).svg"
    svg2emf(filename)

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
    GWdata  = [[], [], []]
    OPRGWdata = [[], [], []]

    for loopcount in range(TotalDataset):

        if loopcount % (n * JXLevel) == 3*n:
            GWdata[0].append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGWdata[0].append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))
        elif loopcount % (n * JXLevel) == 3*n+1:
            GWdata[1].append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGWdata[1].append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))
        elif loopcount % (n * JXLevel) == 3*n+2:
            GWdata[2].append(loadCSFinalPDR(CSNonParaDataset[loopcount]))
            OPRGWdata[2].append(loadCSFinalPDR(CSOPRNonParaDataset[loopcount]))

    ZippedGW = zip(GWdata[0], GWdata[1], GWdata[2])
    ZippedOPRGW = zip(OPRGWdata[0], OPRGWdata[1], OPRGWdata[2])

    y1 = [(a + b + c) / len(GWdata) for a, b, c in ZippedGW]
    y2 = [(a + b + c) / len(OPRGWdata) for a, b, c in ZippedOPRGW]

    # Initialize subplots
    fig, ax1 = plt.subplots()

    # Initialize subplot1 yxis
    ax1.set_ylim((0, 1))

    # Draw two lines
    ax1.plot(x, y1, color='r', marker='x', markersize=7, markeredgecolor='r',
             label='ReLoRaWAN')
    ax1.plot(x, y2, color='b', marker='o', markersize=7, markeredgecolor='b',
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
    filename = "bin/PDR(GWNum)(JX=8).svg"
    svg2emf(filename)

    # Show subplots
    plt.show()

    return
