import numpy as np

JXNum = 6
GWNum = 6
SFNum = 6
TPNum = 8

def initCSJXNum():
    CSJXNumDataset = []
    for loopcount in range(JXNum+1):
        CSJXNumDataset.append('data/nonpower/CS/JXNum/' + str(loopcount) + '/data.csv')
    return CSJXNumDataset


def initCSPara():
    CSParaDataset = []
    for loopcount in range(7, 7 + SFNum, 1):
        for loopcount2 in range(TPNum):
            CSParaDataset.append('data/nonpower/CS/Parameters/SF' + str(loopcount) + 'TP' + str(loopcount2) + '/data.csv')
    return CSParaDataset


def initNSJXNum():
    NSJXNumDataset = []
    for loopcount in range(JXNum+1):
        NSJXNumDataset.append('data/nonpower/NS/JXNum/' + str(loopcount) + '/data.csv')
    return NSJXNumDataset


def initECJXNum():
    ECJXNumDataset = []
    for loopcount in range(JXNum+1):
        ECJXNumDataset.append('data/power/JXNum/' + str(loopcount) + '/power.csv')
    return ECJXNumDataset


def loadPDR(dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(6, 4), unpack=True)
    return x, y


def loadCSThroughput(dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(6, 7), unpack=True)
    return x, y


def loadNSThroughput(dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(0, 1), unpack=True)
    return x, y


def loadEC(dataset):
    (x, voltage, current) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(0, 3, 10), unpack=True)
    y = voltage * current
    return x, y
