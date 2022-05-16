import numpy as np
from scipy import integrate


# https://electronics.stackexchange.com/q/84537
def calAvgNum(yaxis, xaxis):
    integral = integrate.simps(yaxis, xaxis)  # http://python.86x.net/scipy18/index.html
    # integral = integrate.trapezoid(yaxis, xaxis)  # https://zhuanlan.zhihu.com/p/367067235
    # integral = integrate.trapz(yaxis, xaxis)  # https://www.codeleading.com/article/52461047397/

    time = xaxis[len(xaxis) - 1]

    averagenumber = integral / time
    return averagenumber
