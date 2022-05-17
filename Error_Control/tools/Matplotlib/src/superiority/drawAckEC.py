import matplotlib.pyplot as plt
import numpy as np

from scipy import integrate

from src.tool.calAvgNum import calAvgNum


def drawEC():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Draw one subplot
    fig, ax1 = plt.subplots()

    # Load corrected1 data (120s)
    (x1, voltage1, current1) = np.loadtxt('data/experimental/power/JXNum/1/power.csv', skiprows=1, delimiter=',', usecols=(0, 3, 10), unpack=True)
    y1 = voltage1 * current1

    # Load corrected2 data
    (x2, voltage2, current2) = np.loadtxt('data/experimental/power/JXNum/2/power.csv', skiprows=1, delimiter=',', usecols=(0, 3, 10), unpack=True)
    y2 = voltage2 * current2

    # Calculate corrected average power
    averagePower1 = calAvgNum(y1, x1)
    averagePowerPoints1 = np.linspace(averagePower1, averagePower1, len(x1))

    # Calculate original average power
    averagePower2 = calAvgNum(y2, x2)
    averagePowerPoints2 = np.linspace(averagePower2, averagePower2, len(x2))

    # Initialize axis
    ax1.set_ylim((200, 1000))
    ax1.set_xlabel(r'Time (s)')
    ax1.set_ylabel(r'Power (mW)')

    # Draw lines
    l1, = ax1.plot(x1, y1, color='r')
    l2, = ax1.plot(x1, averagePowerPoints1, color='b', linestyle='dashed')
    l3, = ax1.plot(x2, y2, color='y')
    l4, = ax1.plot(x2, averagePowerPoints2, color='g', linestyle='dashed')

    # Draw a vertical connection for annotation
    plt.annotate('',  # 文本内容
                 xy=(x1[int(len(x1) / 2)], averagePowerPoints2[int(len(averagePowerPoints1) / 2)]),  # 注释所在地
                 xytext=(x1[int(len(x1) / 2)], averagePowerPoints1[int(len(averagePowerPoints1) / 2)]), fontsize=16,
                 # 文本所在地
                 arrowprops=dict(arrowstyle="<->", shrinkA=0.05, shrinkB=0.05, color="0.5"))

    # Draw an annotation
    ax1.annotate(r'Offset', xy=(x1[int(len(x1) / 2)], (
            averagePowerPoints1[int(len(averagePowerPoints1) / 2)] + averagePowerPoints2[
        int(len(averagePowerPoints2) / 2)]) / 2),
                 xycoords='data', xytext=(+30, -30),
                 textcoords='offset points', arrowprops=dict(arrowstyle='->', connectionstyle='arc3, rad=.2'))

    # Draw a legend
    plt.legend(handles=[l1, l2, l3, l4, ],
               labels=[r'Instant (JX=2)', r'Average (JX=2)', r'Instant (JX=1)', r'Average (JX=1)', ],
               loc='best',
               prop={'size': 8},
               ncol=2)

    # Save subplots to files
    plt.savefig("bin/EnergyConsumption.pdf", format="pdf", transparent="ture")  # latex

    # Draw title
    # plt.title(r'Energy Consumption')

    # Display subplots
    plt.show()
    return