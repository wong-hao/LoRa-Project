from matplotlib import pyplot as plt

from src.tool.dataset import initNonSNRStack, initEFLoRa, loadEDFlag, loadMinEE, MidDataset


def drawMinEE():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize datasets
    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    ED = 0
    Flag = loadEDFlag(NonSNRStackDataset[MidDataset])
    (x1, y1) = loadMinEE(NonSNRStackDataset[MidDataset])
    (x2, y2) = loadMinEE(EFLoRaDataset[MidDataset])

    # Extract data of ED
    Flaglocation = []
    for loopcount in range(len(Flag)):
        if Flag[loopcount] == ED:
            Flaglocation.append(loopcount)

    realx1 = []
    realy1 = []
    realx2 = []
    realy2 = []

    for i in range(0, len(x1)):
        for j in range(0, len(Flaglocation)):
            if i == Flaglocation[j]:
                realx1.append(x1[i])
    for i in range(0, len(y1)):
        for j in range(0, len(Flaglocation)):
            if i == Flaglocation[j]:
                realy1.append(y1[i])
    for i in range(0, len(x2)):
        for j in range(0, len(Flaglocation)):
            if i == Flaglocation[j]:
                realx2.append(x2[i])
    for i in range(0, len(y2)):
        for j in range(0, len(Flaglocation)):
            if i == Flaglocation[j]:
                realy2.append(y2[i])

    # Initialize subplots
    fig, ax1 = plt.subplots()

    # Initialize axis
    ax1.ticklabel_format(axis="both", style="sci", scilimits=(0, 0))
    ax1.set_xlabel(r'Time (ms)', fontsize=15)
    ax1.set_ylabel(r'Energy Efficiency (bit/mJ)', fontsize=15)
    ax1.set_ylim(0, 1.2 * max(max(y1), max(y2)))

    # Draw lines
    ax1.plot(realx1, realy1, color='r', label=r'EELoRa')
    ax1.plot(realx2, realy2, color='b', label=r'EFLoRa')

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw legends
    plt.legend(loc='best',
               fontsize=15,
               ncol=2)

    # Draw gridlines
    ax1.grid()

    # Save subplots to files
    plt.savefig("bin/MinEE(Convergence).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return
