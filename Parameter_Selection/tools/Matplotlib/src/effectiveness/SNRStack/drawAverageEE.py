
from matplotlib import pyplot as plt

from src.tool.dataset import initSNRStack,  loadEDjAverageEE,  loadEDFlag
from src.tool.formatfile import svg2emf
from src.tool.formatnum import Millisecond2Second, kilo


def drawEffAverageEE():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize datasets
    SNRStackDataset = initSNRStack()

    # Load datasets
    ED1 = 9
    ED2 = 1
    ED3 = 2
    ED4 = 8
    SNRDistanceLevel1 = 0
    SNRDistanceLevel2 = 1
    SNRDistanceLevel3 = 2
    SNRDistanceLevel4 = 3
    Flag1 = loadEDFlag(SNRStackDataset[SNRDistanceLevel1])
    Flag2 = loadEDFlag(SNRStackDataset[SNRDistanceLevel2])
    Flag3 = loadEDFlag(SNRStackDataset[SNRDistanceLevel3])
    Flag4 = loadEDFlag(SNRStackDataset[SNRDistanceLevel4])
    (x1, y1) = loadEDjAverageEE(ED1, SNRStackDataset[SNRDistanceLevel1])
    (x2, y2) = loadEDjAverageEE(ED2, SNRStackDataset[SNRDistanceLevel2])
    (x3, y3) = loadEDjAverageEE(ED3, SNRStackDataset[SNRDistanceLevel3])
    (x4, y4) = loadEDjAverageEE(ED4, SNRStackDataset[SNRDistanceLevel4])

    # Extract data of ED
    Flaglocation1 = []
    Flaglocation2 = []
    Flaglocation3 = []
    Flaglocation4 = []
    for loopcount in range(len(Flag1)):
        if Flag1[loopcount] == ED1:
            Flaglocation1.append(loopcount)
    for loopcount in range(len(Flag2)):
        if Flag2[loopcount] == ED2:
            Flaglocation2.append(loopcount)
    for loopcount in range(len(Flag3)):
        if Flag3[loopcount] == ED3:
            Flaglocation3.append(loopcount)
    for loopcount in range(len(Flag4)):
        if Flag4[loopcount] == ED4:
            Flaglocation4.append(loopcount)

    realx1 = []
    realy1 = []
    realx2 = []
    realy2 = []
    realx3 = []
    realy3 = []
    realx4 = []
    realy4 = []

    for i in range(0, len(x1)):
        for j in range(0, len(Flaglocation1)):
            if i == Flaglocation1[j]:
                realx1.append(x1[i])
    for i in range(0, len(y1)):
        for j in range(0, len(Flaglocation1)):
            if i == Flaglocation1[j]:
                realy1.append(y1[i])

    for i in range(0, len(x2)):
        for j in range(0, len(Flaglocation2)):
            if i == Flaglocation2[j]:
                realx2.append(x2[i])
    for i in range(0, len(y2)):
        for j in range(0, len(Flaglocation2)):
            if i == Flaglocation2[j]:
                realy2.append(y2[i])

    for i in range(0, len(x3)):
        for j in range(0, len(Flaglocation3)):
            if i == Flaglocation3[j]:
                realx3.append(x3[i])
    for i in range(0, len(y3)):
        for j in range(0, len(Flaglocation3)):
            if i == Flaglocation3[j]:
                realy3.append(y3[i])

    for i in range(0, len(x4)):
        for j in range(0, len(Flaglocation4)):
            if i == Flaglocation4[j]:
                realx4.append(x4[i])
    for i in range(0, len(y4)):
        for j in range(0, len(Flaglocation4)):
            if i == Flaglocation4[j]:
                realy4.append(y4[i])

    # Initialize subplots
    fig, ax1 = plt.subplots()

    # Initialize axis
    # ax1.ticklabel_format(axis="both", style="sci", scilimits=(0, 0))
    ax1.set_xlabel(r'Time (s)', fontsize=15)
    ax1.set_ylabel(r'Energy efficiency (bit/mJ)', fontsize=15)

    # Draw lines
    ax1.plot(Millisecond2Second(realx1), realy1, color='r', label=r'n=5')
    ax1.plot(Millisecond2Second(realx2), realy2, color='b', label=r'n=10')
    ax1.plot(Millisecond2Second(realx3), realy3, color='g', label=r'n=15')
    ax1.plot(Millisecond2Second(realx4), realy4, color='y', label=r'n=20')

    ax1.set_xlim(0, min(max(realx1), max(realx2), max(realx3), max(realx4)))

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw legends
    plt.legend(loc='best',
               fontsize=15,
               ncol=2)

    # Draw gridlines
    ax1.grid()

    # Save subplots to files
    filename = "bin1/AverageEE(Distance).svg"
    svg2emf(filename)

    # Show subplots
    plt.show()

    return
