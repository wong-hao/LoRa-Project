import numpy as np
from matplotlib import pyplot as plt

from src.tool.dataset import initInterval, IntervalLevel, loadFinalMinEE, initPower, RealEDNum, PowerLevel, \
    loadMaximumCurrent


def getTP():
    # Choose font
    plt.rc('font', family='Times New Roman')

    # Initialize datasets
    Powerdataset = initPower()

    # Load datasets
    x = np.linspace(1, PowerLevel, PowerLevel)
    Volt = 5
    CurrentTP0 = []
    CurrentTP1 = []
    CurrentTP2 = []
    CurrentTP3 = []
    CurrentTP4 = []
    CurrentTP5 = []
    CurrentTP6 = []
    CurrentTP7 = []

    for loopcount in range(RealEDNum * PowerLevel):
        if loopcount % PowerLevel == 0:
            CurrentTP0.append(loadMaximumCurrent(Powerdataset[loopcount]))
        if loopcount % PowerLevel == 1:
            CurrentTP1.append(loadMaximumCurrent(Powerdataset[loopcount]))
        if loopcount % PowerLevel == 2:
            CurrentTP2.append(loadMaximumCurrent(Powerdataset[loopcount]))
        if loopcount % PowerLevel == 3:
            CurrentTP3.append(loadMaximumCurrent(Powerdataset[loopcount]))
        if loopcount % PowerLevel == 4:
            CurrentTP4.append(loadMaximumCurrent(Powerdataset[loopcount]))
        if loopcount % PowerLevel == 5:
            CurrentTP5.append(loadMaximumCurrent(Powerdataset[loopcount]))
        if loopcount % PowerLevel == 6:
            CurrentTP6.append(loadMaximumCurrent(Powerdataset[loopcount]))
        if loopcount % PowerLevel == 7:
            CurrentTP7.append(loadMaximumCurrent(Powerdataset[loopcount]))

    TP0 = [round((i * Volt), 2) for i in CurrentTP0]
    TP1 = [round((i * Volt), 2) for i in CurrentTP1]
    TP2 = [round((i * Volt), 2) for i in CurrentTP2]
    TP3 = [round((i * Volt), 2) for i in CurrentTP3]
    TP4 = [round((i * Volt), 2) for i in CurrentTP4]
    TP5 = [round((i * Volt), 2) for i in CurrentTP5]
    TP6 = [round((i * Volt), 2) for i in CurrentTP6]
    TP7 = [round((i * Volt), 2) for i in CurrentTP7]

    # print(TP0)
    # print(TP1)
    # print(TP2)
    # print(TP3)
    # print(TP4)
    # print(TP5)
    # print(TP6)
    # print(TP7)

    AverageCurrentTP0 = np.mean(CurrentTP0)
    AverageCurrentTP1 = np.mean(CurrentTP1)
    AverageCurrentTP2 = np.mean(CurrentTP2)
    AverageCurrentTP3 = np.mean(CurrentTP3)
    AverageCurrentTP4 = np.mean(CurrentTP4)
    AverageCurrentTP5 = np.mean(CurrentTP5)
    AverageCurrentTP6 = np.mean(CurrentTP6)
    AverageCurrentTP7 = np.mean(CurrentTP7)

    # print(AverageCurrentTP0)
    # print(AverageCurrentTP1)
    # print(AverageCurrentTP2)
    # print(AverageCurrentTP3)
    # print(AverageCurrentTP4)
    # print(AverageCurrentTP5)
    # print(AverageCurrentTP6)
    # print(AverageCurrentTP7)

    AverageTP0 = round(AverageCurrentTP0*Volt, 2)
    AverageTP1 = round(AverageCurrentTP1*Volt, 2)
    AverageTP2 = round(AverageCurrentTP2*Volt, 2)
    AverageTP3 = round(AverageCurrentTP3*Volt, 2)
    AverageTP4 = round(AverageCurrentTP4*Volt, 2)
    AverageTP5 = round(AverageCurrentTP5*Volt, 2)
    AverageTP6 = round(AverageCurrentTP6*Volt, 2)
    AverageTP7 = round(AverageCurrentTP7*Volt, 2)

    # print(AverageTP0)
    # print(AverageTP1)
    # print(AverageTP2)
    # print(AverageTP3)
    # print(AverageTP4)
    # print(AverageTP5)
    # print(AverageTP6)
    # print(AverageTP7)

    return
