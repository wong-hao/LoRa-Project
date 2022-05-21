import matplotlib.pyplot as plt
import numpy as np

from scipy import integrate

from src.tool.Avg import calAvgNum, getAvg
from src.tool.Dataset import loadEC, initECJXNum


def drawEC():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Draw one subplot
    fig, ax1 = plt.subplots()

    # Load datasets (120s)
    ECJXNumDataset = initECJXNum()

    # Load datasets
    (x1, y1) = loadEC(ECJXNumDataset[1])
    (x2, y2) = loadEC(ECJXNumDataset[2])

    # Calculate corrected average power
    (averagePower1, averagePowerPoints1) = getAvg((x1, y1))

    # Calculate original average power
    (averagePower2, averagePowerPoints2) = getAvg((x2, y2))

    # Initialize axis
    ax1.set_ylim(0.8*min(min(y1), min(y2)), 1.2*max(max(y1), max(y2)))
    ax1.set_xlabel(r'Time (s)', fontsize=15)
    ax1.set_ylabel(r'Power (mW)', fontsize=15)

    # Choose tick pramaters
    ax1.tick_params(labelsize=15)

    # Draw lines
    ax1.plot(x1, y1, color='r', label=r'Instant (Mid Traffic)')
    ax1.plot(x1, averagePowerPoints1, color='b', linestyle='dashed', label=r'Average (Mid Traffic)')
    ax1.plot(x2, y2, color='y', label=r'Instant (Low Traffic)')
    ax1.plot(x2, averagePowerPoints2, color='g', linestyle='dashed', label=r'Average (Low Traffic)')

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

    # Draw a legend
    plt.legend(loc='best',
               prop={'size': 13},
               ncol=2)

    # Save subplots to files
    plt.savefig("bin/EnergyConsumption.pdf", format="pdf", transparent="ture")  # latex

    # Draw title
    # plt.title(r'Energy Consumption')

    # Display subplots
    plt.show()
    return
