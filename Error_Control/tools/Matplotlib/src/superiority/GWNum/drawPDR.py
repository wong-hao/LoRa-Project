import matplotlib.pyplot as plt
import numpy as np

from src.tool import Dataset
from src.tool.Avg import getAvg
from src.tool.Dataset import initCSNonPara, LoadCSPDR, initCSOPRNonPara

def drawSupCSPDRJX2():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Load data
    x = np.linspace(Dataset.GWNumMin, Dataset.GWNumMax, Dataset.GWNum)

    CSNonParaDataset = initCSNonPara()
    CSOPRNonParaDataset = initCSOPRNonPara()

    GW0 = []
    OPRGW0 = []

    for loopcount in range(Dataset.GWNum * Dataset.JXLevel):
        (averagePDR, averagePDRPoints) = getAvg(LoadCSPDR(CSNonParaDataset[loopcount]))
        (averageOPRPDR, averageOPRPDRPoints) = getAvg(LoadCSPDR(CSOPRNonParaDataset[loopcount]))

        if loopcount % Dataset.JXLevel == 0:
            GW0.append(averagePDR)
            OPRGW0.append(averageOPRPDR)

    print(GW0)
    print(OPRGW0)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Initialize subplot1 yxis
    ax1.set_ylim((0, 1))

    '''
    # Draw two error charts
    yerr = [0, 0, 0, 0, 0]
    ax1.errorbar(x, GW0, yerr=yerr, ecolor='black', color='b', capsize=4, marker='x', markersize=7, markeredgecolor='b',
                 label='CCLoRa')
    ax1.errorbar(x, OPRGW0, yerr=yerr, ecolor='black', color='r', capsize=4, marker='o', markersize=7,
                 markeredgecolor='r', label='OPR')
    '''

    # Draw two lines
    ax1.plot(x, GW0, color='r', marker='x', markersize=7, markeredgecolor='r',
                 label='CCLoRa')
    ax1.plot(x, OPRGW0, color='b', marker='o', markersize=7, markeredgecolor='b',
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
    plt.savefig("bin/PDR(Dataset.GWNum)(JX=2).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return

def drawSupCSPDRJX4():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Load data
    x = np.linspace(Dataset.GWNumMin, Dataset.GWNumMax, Dataset.GWNum)

    CSNonParaDataset = initCSNonPara()
    CSOPRNonParaDataset = initCSOPRNonPara()

    GW1 = []
    OPRGW1 = []

    for loopcount in range(Dataset.GWNum * Dataset.JXLevel):
        (averagePDR, averagePDRPoints) = getAvg(LoadCSPDR(CSNonParaDataset[loopcount]))
        (averageOPRPDR, averageOPRPDRPoints) = getAvg(LoadCSPDR(CSOPRNonParaDataset[loopcount]))

        if loopcount % Dataset.JXLevel == 1:
            GW1.append(averagePDR)
            OPRGW1.append(averageOPRPDR)

    print(GW1)
    print(OPRGW1)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Initialize subplot1 yxis
    ax1.set_ylim((0, 1))


    # Draw two lines
    ax1.plot(x, GW1, color='r', marker='x', markersize=7, markeredgecolor='r',
                 label='CCLoRa')
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
    plt.savefig("bin/PDR(Dataset.GWNum)(JX=4).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return

def drawSupCSPDRJX6():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Load data
    x = np.linspace(Dataset.GWNumMin, Dataset.GWNumMax, Dataset.GWNum)

    CSNonParaDataset = initCSNonPara()
    CSOPRNonParaDataset = initCSOPRNonPara()

    GW2 = []
    OPRGW2 = []

    for loopcount in range(Dataset.GWNum * Dataset.JXLevel):
        (averagePDR, averagePDRPoints) = getAvg(LoadCSPDR(CSNonParaDataset[loopcount]))
        (averageOPRPDR, averageOPRPDRPoints) = getAvg(LoadCSPDR(CSOPRNonParaDataset[loopcount]))

        if loopcount % Dataset.JXLevel == 2:
            GW2.append(averagePDR)
            OPRGW2.append(averageOPRPDR)


    print(GW2)
    print(OPRGW2)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Initialize subplot1 yxis
    ax1.set_ylim((0, 1))

    # Draw two lines
    ax1.plot(x, GW2, color='r', marker='x', markersize=7, markeredgecolor='r',
                 label='CCLoRa')
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
    plt.savefig("bin/PDR(Dataset.GWNum)(JX=6).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return

def drawSupCSPDRJX8():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Load data
    x = np.linspace(Dataset.GWNumMin, Dataset.GWNumMax, Dataset.GWNum)

    CSNonParaDataset = initCSNonPara()
    CSOPRNonParaDataset = initCSOPRNonPara()

    GW3 = []
    OPRGW3 = []

    for loopcount in range(Dataset.GWNum * Dataset.JXLevel):
        (averagePDR, averagePDRPoints) = getAvg(LoadCSPDR(CSNonParaDataset[loopcount]))
        (averageOPRPDR, averageOPRPDRPoints) = getAvg(LoadCSPDR(CSOPRNonParaDataset[loopcount]))

        if loopcount % Dataset.JXLevel == 3:
            GW3.append(averagePDR)
            OPRGW3.append(averageOPRPDR)


    print(GW3)
    print(OPRGW3)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Initialize subplot1 yxis
    ax1.set_ylim((0, 1))

    # Draw two lines
    ax1.plot(x, GW3, color='r', marker='x', markersize=7, markeredgecolor='r',
                 label='CCLoRa')
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
    plt.savefig("bin/PDR(Dataset.GWNum)(JX=8).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return