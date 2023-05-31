import matplotlib.pyplot as plt

from src.tool.dataset import initNonSNRStack, loadEDFlag, loadFairIndex, TotalDataset, initEFLoRa
from src.tool.formatfile import svg2emf
from src.tool.formatnum import two_decimal_formatter, Millisecond2Second


def drawFairness():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize datasets
    NonSNRStackDataset = initNonSNRStack()
    EFLoRaDataset = initEFLoRa()

    # Load datasets
    ED1 = 5
    Flag1 = loadEDFlag(NonSNRStackDataset[TotalDataset-1])
    ED2 = 0
    Flag2 = loadEDFlag(EFLoRaDataset[TotalDataset-8])
    (x1, y1) = loadFairIndex(NonSNRStackDataset[TotalDataset-1])
    (x2, y2) = loadFairIndex(EFLoRaDataset[TotalDataset-8])

    # Extract data of ED
    Flaglocation1 = []
    for loopcount in range(len(Flag2)):
        if Flag1[loopcount] == ED1:
            Flaglocation1.append(loopcount)
    Flaglocation2 = []
    for loopcount in range(len(Flag2)):
        if Flag2[loopcount] == ED2:
            Flaglocation2.append(loopcount)

    realx1 = []
    realy1 = []
    realx2 = []
    realy2 = []

    for i in range(0, len(x1)):
        for j in range(0, len(Flaglocation1)):
            if i == Flaglocation1[j]:
                realx1.append(x1[i])
    for i in range(0, len(y1)):
        for j in range(0, len(Flaglocation1)):
            if i == Flaglocation1[j]:
                realy1.append(y1[i])
    for i in range(0, len(x2)):
        for j in range(0, len(Flaglocation1)):
            if i == Flaglocation2[j]:
                realx2.append(x2[i])
    for i in range(0, len(y2)):
        for j in range(0, len(Flaglocation1)):
            if i == Flaglocation2[j]:
                realy2.append(y2[i])

    # Initialize subplots
    fig, ax1 = plt.subplots()

    # Initialize axis
    # ax1.ticklabel_format(axis="both", style="sci", scilimits=(0, 0))
    ax1.set_xlabel(r'Time (s)', fontsize=15)
    ax1.set_ylabel('Jain\'s Fairness Index', fontsize=15)
    ax1.set_ylim(0, 1.05)

    # Draw lines
    ax1.plot(Millisecond2Second(realx1), realy1, label='EEADR', color='r')
    print(realy1)
    ax1.plot(Millisecond2Second(realx2), realy2, label='EF-LoRa', color='b')

    # ax1.set_xlim(0, min(max(realx1), max(realx2)))
    ax1.set_xlim(0, 1200)
    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Set y axis format with two decimal places
    formatter = two_decimal_formatter()
    ax1.yaxis.set_major_formatter(formatter)

    # Draw gridlines
    ax1.grid()

    # Draw legends
    plt.legend(loc='best',
               fontsize=15,
               ncol=3)

    # Save subplots to files
    filename = "bin1/Fairness.svg"
    svg2emf(filename)

    # Show subplots
    plt.show()

    return
