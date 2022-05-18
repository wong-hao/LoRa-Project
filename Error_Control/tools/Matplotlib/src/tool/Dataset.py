import numpy as np


def initGWJXNum():
    GWJXNumDataset = []
    for loopcount in range(7):
        GWJXNumDataset.append('data/nonpower/GW/JXNum/' + str(loopcount) + '/data.csv')
    return GWJXNumDataset


def initNSJXNum():
    NSJXNumDataset = []
    for loopcount in range(7):
        NSJXNumDataset.append('data/nonpower/NS/JXNum/' + str(loopcount) + '/data.csv')
    return NSJXNumDataset


def initECJXNum():
    ECJXNumDataset = []
    for loopcount in range(7):
        ECJXNumDataset.append('data/power/JXNum/' + str(loopcount) + '/power.csv')
    return ECJXNumDataset


def loadPDR(dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(6, 4), unpack=True)
    return x, y


def loadGWThroughput(dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(6, 7), unpack=True)
    return x, y


def loadNSThroughput(dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(0, 1), unpack=True)
    return x, y


def loadEC(dataset):
    (x, voltage, current) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(0, 3, 10), unpack=True)
    y = voltage * current
    return x, y
