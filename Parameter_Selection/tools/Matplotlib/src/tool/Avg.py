import numpy as np
from scipy import integrate


# https://electronics.stackexchange.com/q/84537
def calAvgNum(x, y):  # x -> time, y -> value
    integral = integrate.simps(y, x)  # https://www.codeleading.com/article/52461047397
    # integral = integrate.trapezoid(y, x)  # https://zhuanlan.zhihu.com/p/367067235
    # integral = integrate.trapz(y, x)  # http://python.86x.net/scipy18/index.html

    time = x[len(x) - 1]

    averagenumber = integral / time

    return averagenumber


def getAvgNum(array):
    avgnum = calAvgNum(array[0], array[1])

    return avgnum


def getAvgPoints(array):
    avgnum = calAvgNum(array[0], array[1])
    averagepoints = np.linspace(avgnum, avgnum, len(array[0]))

    return averagepoints