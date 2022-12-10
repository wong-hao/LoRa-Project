from matplotlib import pyplot as plt

from src.tool.dataset import initSNRStack, loadEDjInstantEE, loadEDjAverageEE, RealEDNum

ED = 0
SNRDistanceLevel = 0

def drawDistance():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Init datasets
    SNRStackDataset = initSNRStack()

    # Load datasets
    (x1, y1) = loadEDjInstantEE(ED, SNRStackDataset[SNRDistanceLevel])
    (x2, y2) = loadEDjAverageEE(ED, SNRStackDataset[SNRDistanceLevel])

    # Extract data of ED
    realx1 = []
    realy1 = []
    realx2 = []
    realy2 = []
    for i in range(len(x1)):
        if i % RealEDNum == ED:
            realx1.append(x1[i])
    for i in range(len(y1)):
        if i % RealEDNum == ED:
            realy1.append(y1[i])
    for i in range(len(x2)):
        if i % RealEDNum == ED:
            realx2.append(x2[i])
    for i in range(len(y2)):
        if i % RealEDNum == ED:
            realy2.append(y2[i])


    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Initialize axis
    ax1.ticklabel_format(axis="both", style="sci", scilimits=(0, 0))
    ax1.set_xlabel(r'Time (ms)', fontsize=15)
    ax1.set_ylabel(r'Energy Efficiency (bit/mj)', fontsize=15)

    # Draw lines
    ax1.plot(realx1, realy1, color='r', label=r'Instant EE')
    ax1.plot(realx2, realy2, color='b', label=r'Average EE')

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw legends
    plt.legend(loc='best',
               fontsize=15,
               ncol=2)

    # Draw gridlines
    ax1.grid()

    # Save subplots to files
    plt.savefig("bin/Distance (Level = 0).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return