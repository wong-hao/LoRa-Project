import matplotlib.pyplot as plt
import numpy as np

from scipy import integrate

def drawEC():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Draw one subplot
    fig, ax1 = plt.subplots()

    # Load data
    x = np.loadtxt('data/power.csv', skiprows=1, delimiter=',', usecols=0, unpack=True)
    y1 = np.loadtxt('data/power.csv', skiprows=1, delimiter=',', usecols=2, unpack=True)
    y2 = np.loadtxt('data/power.csv', skiprows=1, delimiter=',', usecols=3, unpack=True)
    instantPower = y1 * y2

    # Calculate average power (https://electronics.stackexchange.com/q/84537)
    integral = integrate.simps(instantPower, x)  # http://python.86x.net/scipy18/index.html
    # integral = integrate.trapezoid(power, x)  # https://zhuanlan.zhihu.com/p/367067235
    # integral = integrate.trapz(power, x))  # https://www.codeleading.com/article/52461047397/
    time = x[len(x)-1]
    averagePower = integral/time
    averagePowerPoints = np.linspace(averagePower, averagePower, len(x))

    # Initialize axis
    ax1.set_xlabel(r'Time (s)')
    ax1.set_ylabel(r'Power (mW)')

    # Draw lines
    l1, = ax1.plot(x, instantPower, color='r')
    l2, = ax1.plot(x, averagePowerPoints, color='b')

    # Draw a legend
    plt.legend(handles=[l1, l2, ], labels=[r'Instant Power (on)', r'Average Power (on)'], loc='best')

    # Save subplots to files
    plt.savefig("bin/energy.pdf", format="pdf", transparent="ture")  # latex

    # Display subplots
    plt.show()
    return
