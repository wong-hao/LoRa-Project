from matplotlib import pyplot as plt

# Draw a parallel histogram
from matplotlib.ticker import FuncFormatter
import matplotlib.ticker as mtick

from src.tool.Avg import getAvg
from src.tool.Dataset import loadNSThroughput, loadCSThroughput, initCSNonPara, initNSNonPara, MidDataset

TX_INTERVAL = 10
pendTxLen = 28

def drawInstantThroughput():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Theoretical upper limit of throughput
    # theory = pendTxLen * 8 / (TX_INTERVAL * 1000)

    # Init datasets
    CSNonParaDataset = initCSNonPara()
    NSNonParaDataset = initNSNonPara()

    # Load datasets
    (x1, y1) = loadCSThroughput(CSNonParaDataset[MidDataset])
    (x2, y2) = loadNSThroughput(NSNonParaDataset[MidDataset])
    list_2 = list[0:len(x1):1]
    print(list_2)

    # Calculate average throughput1
    (averagethroughput1, averagethroughputPoints1) = getAvg((x1, y1))

    # Calculate average throughput2
    (averagethroughput2, averagethroughputPoints2) = getAvg((x2, y2))

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Initialize axis
    # ax1.set_ylim((0, 1))
    ax1.ticklabel_format(axis="both", style="sci", scilimits=(0, 0))
    ax1.set_xlabel(r'Time (ms)', fontsize=15)
    ax1.set_ylabel(r'Throughput (kbps)', fontsize=15)

    # Draw lines
    ax1.plot(x1, y1, color='r', label=r'CS')
    ax1.plot(x2, y2, color='b', label=r'AS', linestyle='dashed')

    # ax1.plot(x1, averagethroughputPoints1, color='g', linestyle="--", label=r'Average (CS)')
    # ax1.plot(x2, averagethroughputPoints2, color='y', linestyle="--", label=r'Average (NS)')

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw legends
    plt.legend(loc='best',
               fontsize=15,
               ncol=2)

    # Draw gridlines
    ax1.grid()

    # Draw title
    # plt.title(r'Instant Throughput')

    # Save subplots to files
    plt.savefig("bin/Throughput(Instant).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return
