from matplotlib import pyplot as plt

from src.tool.dataset import initSNRStack, loadEDjInstantEE, loadEDjAverageEE, RealEDNum, loadEDFlag

def drawEffAverageEE5():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Init datasets
    SNRStackDataset = initSNRStack()

    # Load datasets
    ED = 0
    SNRDistanceLevel = 0
    Flag = loadEDFlag(SNRStackDataset[SNRDistanceLevel])
    (x1, y1) = loadEDjInstantEE(ED, SNRStackDataset[SNRDistanceLevel])
    (x2, y2) = loadEDjAverageEE(ED, SNRStackDataset[SNRDistanceLevel])

    # Extract data of ED
    Flaglocation = []
    for loopcount in range(len(Flag)):
        if Flag[loopcount] == ED:
            Flaglocation.append(loopcount)

    realx2 = []
    realy2 = []

    for i in range(0, len(x2)):
        for j in range(0, len(Flaglocation)):
            if i == Flaglocation[j]:
                realx2.append(x2[i])
    for i in range(0, len(y2)):
        for j in range(0, len(Flaglocation)):
            if i == Flaglocation[j]:
                realy2.append(y2[i])

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Initialize axis
    ax1.ticklabel_format(axis="both", style="sci", scilimits=(0, 0))
    ax1.set_xlabel(r'Time (ms)', fontsize=15)
    ax1.set_ylabel(r'Energy Efficiency (bit/mj)', fontsize=15)

    # Draw lines
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
    plt.savefig("bin/InstantEE(Distance=0).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return

def drawEffAverageEE10():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Init datasets
    SNRStackDataset = initSNRStack()

    # Load datasets
    ED = 1
    SNRDistanceLevel = 1
    Flag = loadEDFlag(SNRStackDataset[SNRDistanceLevel])
    (x1, y1) = loadEDjInstantEE(ED, SNRStackDataset[SNRDistanceLevel])
    (x2, y2) = loadEDjAverageEE(ED, SNRStackDataset[SNRDistanceLevel])

    # Extract data of ED
    Flaglocation = []
    for loopcount in range(len(Flag)):
        if Flag[loopcount] == ED:
            Flaglocation.append(loopcount)

    realx2 = []
    realy2 = []

    for i in range(0, len(x2)):
        for j in range(0, len(Flaglocation)):
            if i == Flaglocation[j]:
                realx2.append(x2[i])
    for i in range(0, len(y2)):
        for j in range(0, len(Flaglocation)):
            if i == Flaglocation[j]:
                realy2.append(y2[i])

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Initialize axis
    ax1.ticklabel_format(axis="both", style="sci", scilimits=(0, 0))
    ax1.set_xlabel(r'Time (ms)', fontsize=15)
    ax1.set_ylabel(r'Energy Efficiency (bit/mj)', fontsize=15)

    # Draw lines
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
    plt.savefig("bin/AverageEE(Distance=1).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return

def drawEffAverageEE15():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Init datasets
    SNRStackDataset = initSNRStack()

    # Load datasets
    ED = 3
    SNRDistanceLevel = 2
    Flag = loadEDFlag(SNRStackDataset[SNRDistanceLevel])
    (x1, y1) = loadEDjInstantEE(ED, SNRStackDataset[SNRDistanceLevel])
    (x2, y2) = loadEDjAverageEE(ED, SNRStackDataset[SNRDistanceLevel])

    # Extract data of ED
    Flaglocation = []
    for loopcount in range(len(Flag)):
        if Flag[loopcount] == ED:
            Flaglocation.append(loopcount)

    realx2 = []
    realy2 = []

    for i in range(0, len(x2)):
        for j in range(0, len(Flaglocation)):
            if i == Flaglocation[j]:
                realx2.append(x2[i])
    for i in range(0, len(y2)):
        for j in range(0, len(Flaglocation)):
            if i == Flaglocation[j]:
                realy2.append(y2[i])

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Initialize axis
    ax1.ticklabel_format(axis="both", style="sci", scilimits=(0, 0))
    ax1.set_xlabel(r'Time (ms)', fontsize=15)
    ax1.set_ylabel(r'Energy Efficiency (bit/mj)', fontsize=15)

    # Draw lines
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
    plt.savefig("bin/AverageEE(Distance=2).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return

def drawEffAverageEE20():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Init datasets
    SNRStackDataset = initSNRStack()

    # Load datasets
    ED = 1
    SNRDistanceLevel = 3
    Flag = loadEDFlag(SNRStackDataset[SNRDistanceLevel])
    (x1, y1) = loadEDjInstantEE(ED, SNRStackDataset[SNRDistanceLevel])
    (x2, y2) = loadEDjAverageEE(ED, SNRStackDataset[SNRDistanceLevel])

    # Extract data of ED
    Flaglocation = []
    for loopcount in range(len(Flag)):
        if Flag[loopcount] == ED:
            Flaglocation.append(loopcount)

    realx2 = []
    realy2 = []

    for i in range(0, len(x2)):
        for j in range(0, len(Flaglocation)):
            if i == Flaglocation[j]:
                realx2.append(x2[i])
    for i in range(0, len(y2)):
        for j in range(0, len(Flaglocation)):
            if i == Flaglocation[j]:
                realy2.append(y2[i])

    # Initialize subplot
    fig, ax1 = plt.subplots()

    # Initialize axis
    ax1.ticklabel_format(axis="both", style="sci", scilimits=(0, 0))
    ax1.set_xlabel(r'Time (ms)', fontsize=15)
    ax1.set_ylabel(r'Energy Efficiency (bit/mj)', fontsize=15)

    # Draw lines
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
    plt.savefig("bin/AverageEE(Distance=3).pdf", format="pdf", transparent="ture")

    # Show subplots
    plt.show()

    return
