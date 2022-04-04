import matplotlib.pyplot as plt
import numpy as np

from scipy import integrate


def drawEC():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Draw one subplot
    fig, ax1 = plt.subplots()

    # Load corrected data
    x1 = np.loadtxt('data/experimental/1/power.csv', skiprows=1, delimiter=',', usecols=0, unpack=True)
    y1 = np.loadtxt('data/experimental/1/power.csv', skiprows=1, delimiter=',', usecols=2, unpack=True)
    y2 = np.loadtxt('data/experimental/1/power.csv', skiprows=1, delimiter=',', usecols=3, unpack=True)
    instantPower1 = y1 * y2

    # Load original data
    # TODO: record the same time period data without correction
    averagePowerPoints2 = np.linspace(200, 200, len(x1))

    # Calculate average power (https://electronics.stackexchange.com/q/84537)
    integral1 = integrate.simps(instantPower1, x1)  # http://python.86x.net/scipy18/index.html
    # integral1 = integrate.trapezoid(instantPower, x)  # https://zhuanlan.zhihu.com/p/367067235
    # integral1 = integrate.trapz(instantPower, x)  # https://www.codeleading.com/article/52461047397/
    time1 = x1[len(x1) - 1]
    averagePower1 = integral1 / time1
    averagePowerPoints1 = np.linspace(averagePower1, averagePower1, len(x1))

    # Initialize axis
    ax1.set_xlabel(r'Time (s)')
    ax1.set_ylabel(r'Power (mW)')

    # Draw lines
    l1, = ax1.plot(x1, instantPower1, color='r')
    l2, = ax1.plot(x1, averagePowerPoints1, color='b', linestyle='dashed')
    l4, = ax1.plot(x1, averagePowerPoints2, color='g', linestyle='dashed')

    # Draw a vertical line
    # ax1.plot([x1[int(len(x1) / 2)], x1[int(len(x1) / 2)]], [averagePowerPoints1[int(len(averagePowerPoints1) / 2)], averagePowerPoints2[int(len(averagePowerPoints2) / 2)]], color='black',
    #        linestyle="--")
    plt.annotate('',  # 文本内容
                 xy=(x1[int(len(x1) / 2)], averagePowerPoints2[int(len(averagePowerPoints1) / 2)]),  # 注释所在地
                 xytext=(x1[int(len(x1) / 2)], averagePowerPoints1[int(len(averagePowerPoints1) / 2)]), fontsize=16,  # 文本所在地
                 arrowprops=dict(arrowstyle="<->"))

    # Draw an annotation
    ax1.annotate(r'Offset', xy=(x1[int(len(x1) / 2)], (
                averagePowerPoints1[int(len(averagePowerPoints1) / 2)] + averagePowerPoints2[
            int(len(averagePowerPoints2) / 2)]) / 2),
                 xycoords='data', xytext=(+30, -30),
                 textcoords='offset points', arrowprops=dict(arrowstyle='->', connectionstyle='arc3, rad=.2'))

    # Draw a legend
    plt.legend(handles=[l1, l2, l4, ], labels=[r'Instant Power (on)', r'Average Power (on)', r'Average Power (off)', ],
               loc='best')

    # Save subplots to files
    plt.savefig("bin/energy.pdf", format="pdf", transparent="ture")  # latex

    # Display subplots
    plt.show()
    return
