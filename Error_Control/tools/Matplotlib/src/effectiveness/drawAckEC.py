import matplotlib.pyplot as plt
import numpy as np

from scipy import integrate

from src.tool.Avg import calAvgNum, getAvg
from src.tool.Dataset import loadEC, initEC, initOriginalEC


def drawInstantEC():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Draw one subplot
    fig, ax1 = plt.subplots()

    # Load datasets (120s)
    ECDataset = initEC()

    # Load datasets
    (x1, y1) = loadEC(ECDataset[2])
    (x2, y2) = loadEC(ECDataset[3])

    # Initialize axis
    ax1.set_ylim(0.8 * min(min(y1), min(y2)), 1.1 * max(max(y1), max(y2)))
    ax1.set_xlabel(r'Time (s)', fontsize=15)
    ax1.set_ylabel(r'Power (mW)', fontsize=15)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw lines
    ax1.plot(x1, y1, color='r', label=r'Legacy LoRaWAN')
    ax1.plot(x2, y2, color='g', label=r'CCLoRa')

    # Draw a legend
    plt.legend(loc='best',
               prop={'size': 13},
               ncol=2)

    # Save subplots to files
    plt.savefig("bin/EnergyConsumption(Instant).pdf", format="pdf", transparent="ture")  # latex

    # Draw title
    # plt.title(r'Energy Consumption')

    # Display subplots
    plt.show()
    return


def drawAverageEC():
    # Choose font
    plt.rc('font', family='Times New Roman')

    labels = ['Legacy LoRaWAN', 'CCLoRa']

    # Draw one subplot
    fig, ax1 = plt.subplots()

    # Load datasets (120s)
    OriginalECDataset = initOriginalEC()

    # Load datasets
    (x1, y1) = loadEC(OriginalECDataset[2])
    (x2, y2) = loadEC(OriginalECDataset[3])

    # Calculate corrected average power
    (averagePower1, averagePowerPoints1) = getAvg((x1, y1))

    # Calculate original average power
    (averagePower2, averagePowerPoints2) = getAvg((x2, y2))

    '''
    # Draw a vertical connection for annotation
    plt.annotate('',  # 文本内容
                 xy=(x1[int(len(x1) / 2)], averagePowerPoints2[int(len(averagePowerPoints1) / 2)]),  # 注释所在地
                 xytext=(x1[int(len(x1) / 2)], averagePowerPoints1[int(len(averagePowerPoints1) / 2)]), fontsize=16,
                 color='black',
                 # 文本所在地
                 arrowprops=dict(arrowstyle="<->", shrinkA=0.1, shrinkB=0.1, color='black'))

    # Draw an annotation
    ax1.annotate(r'Offset', xy=(x1[int(len(x1) / 2)], (
            averagePowerPoints1[int(len(averagePowerPoints1) / 2)] + averagePowerPoints2[
        int(len(averagePowerPoints2) / 2)]) / 2),
                 xycoords='data', xytext=(+30, -30), color='black',
                 textcoords='offset points', arrowprops=dict(arrowstyle='->', connectionstyle='arc3, rad=.2', color='black'))
    '''

    # Initialize axis
    ax1.set_xlabel(r'Approaches', fontsize=15)
    ax1.set_ylabel(r'Power (mW)', fontsize=15)

    # Draw a bar
    x1 = [0]
    x2 = [1]
    bar_width = 0.5

    plt.bar(x1, averagePower1, bar_width, label=r'Legacy LoRaWAN')
    plt.bar(x2, averagePower2, bar_width, label=r'CCLoRa')

    ticks = [0, 1]
    plt.xticks(ticks, labels)

    # Draw a legend
    plt.legend(loc='best',
               prop={'size': 13},
               ncol=2)

    # Save subplots to files
    plt.savefig("bin/EnergyConsumption(Average).pdf", format="pdf", transparent="ture")  # latex

    # Draw title
    # plt.title(r'Energy Consumption')

    # Display subplots
    plt.show()
    return
