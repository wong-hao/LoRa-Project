import matplotlib.pyplot as plt
import numpy as np

from src.tool.Avg import getAvg
from src.tool.Dataset import initCSNonPara, loadPDR, initCSOPRNonPara

GWNum = 5
JXNum = 3

def drawSupCSPDRMid():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Load data
    x = np.linspace(2, 6, GWNum)

    CSNonParaDataset = initCSNonPara()
    CSOPRNonParaDataset = initCSOPRNonPara()

    GW0 = []
    OPRGW0 = []


    for loopcount in range(GWNum * JXNum):
        (averagePDR, averagePDRPoints) = getAvg(loadPDR(CSNonParaDataset[loopcount]))
        (averageOPRPDR, averageOPRPDRPoints) = getAvg(loadPDR(CSOPRNonParaDataset[loopcount]))

        if loopcount % JXNum == 0:
            GW0.append(averagePDR)
            OPRGW0.append(averageOPRPDR)

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Draw two error charts
    yerr = [0.18, 0.16, 0.14, 0.12, 0.1]
    ax1.errorbar(x, GW0, yerr=yerr, ecolor='black', color='b', capsize=4, marker='x', markersize=7, markeredgecolor='b', label='CLoRa')
    ax1.errorbar(x, OPRGW0, yerr=yerr, ecolor='black', color='r', capsize=4, marker='o', markersize=7, markeredgecolor='r', label='OPR')

    # Draw gridlines
    ax1.grid()

    # Draw legends
    plt.legend(fontsize=15)
    # Draw title
    # plt.title(r'Instant Packet Deliver Ratio')

    # Initialize axis
    ax1.set_xlabel(r'Number of gateways', fontsize=15)
    ax1.set_ylabel(r'Packet Delivery Ratio', fontsize=15)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Save subplots to files
    plt.savefig("bin/PDR(GWNum)(Mid Power).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return
