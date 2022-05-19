import numpy as np

JXNum = 6
GWNum = 6

TPNum = 8
TPLevelNum = 3
TPOption = 1
SFNum = 6


def initCSJXNum():
    CSJXNumDataset = []
    for loopcount in range(JXNum+1):
        CSJXNumDataset.append('data/nonpower/CS/JXNum/' + str(loopcount) + '/data.csv')
    return CSJXNumDataset


def initCSPara():
    global ran
    CSParaDataset = []
    for loopcount in range(7, 7 + SFNum, 1):
        if TPOption == 0:
            ran = TPNum
        elif TPOption ==1:
            ran = TPLevelNum
        else:
            print(r'TPOption is illegal!')
        for loopcount2 in range(ran):
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
