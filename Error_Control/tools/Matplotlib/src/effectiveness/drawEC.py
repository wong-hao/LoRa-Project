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
    y1 = np.loadtxt('data/experimental/1/power.csv', skiprows=1, delimiter=',', usecols=11, unpack=True)

    # Load original data
    # TODO: record the same time period data without correction
    x2 = np.loadtxt('data/experimental/2/power.csv', skiprows=1, delimiter=',', usecols=0, unpack=True)
    y2 = np.loadtxt('data/experimental/2/power.csv', skiprows=1, delimiter=',', usecols=11, unpack=True)

    # Calculate corrected average power (https://electronics.stackexchange.com/q/84537)
    integral1 = integrate.simps(y1, x1)  # http://python.86x.net/scipy18/index.html
    # integral1 = integrate.trapezoid(y1, x1)  # https://zhuanlan.zhihu.com/p/367067235
    # integral1 = integrate.trapz(y1, x1)  # https://www.codeleading.com/article/52461047397/
    time1 = x1[len(x1) - 1]
    averagePower1 = integral1 / time1
    averagePowerPoints1 = np.linspace(averagePower1, averagePower1, len(x1))

    # Calculate original average power
    # TODO: record the same time period data without correction
    integral2 = integrate.simps(y2, x2)  # http://python.86x.net/scipy18/index.html
    # integral2 = integrate.trapezoid(y2, x2)  # https://zhuanlan.zhihu.com/p/367067235
    # integral2 = integrate.trapz(y2, x2)  # https://www.codeleading.com/article/52461047397/
    time2 = x2[len(x2) - 1]
    averagePower2 = integral2 / time2
    averagePowerPoints2 = np.linspace(averagePower2, averagePower2, len(x2))

    # Initialize axis
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
                 xytext=(x1[int(len(x1) / 2)], averagePowerPoints1[int(len(averagePowerPoints1) / 2)]), fontsize=16,  # 文本所在地
                 arrowprops=dict(arrowstyle="<->", shrinkA=0.05, shrinkB=0.05, color="0.5"))

    # Draw an annotation
    ax1.annotate(r'Offset', xy=(x1[int(len(x1) / 2)], (
                averagePowerPoints1[int(len(averagePowerPoints1) / 2)] + averagePowerPoints2[
            int(len(averagePowerPoints2) / 2)]) / 2),
                 xycoords='data', xytext=(+30, -30),
                 textcoords='offset points', arrowprops=dict(arrowstyle='->', connectionstyle='arc3, rad=.2'))

    # Draw a legend
    plt.legend(handles=[l1, l2, l3, l4, ], labels=[r'Instant Power (on)', r'Average Power (on)', r'Instant Power (off)', r'Average Power (off)', ],
               loc='best')

    # Save subplots to files
    plt.savefig("bin/energy.pdf", format="pdf", transparent="ture")  # latex

    # Draw title
    plt.title(r'Energy Consumption')

    # Display subplots
    plt.show()
    return
