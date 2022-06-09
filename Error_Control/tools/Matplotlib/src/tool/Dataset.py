import numpy as np

GWNum = 5
JXNum = 3

TPNum = 8
TPLevelNum = 3
SFNum = 6

def initCSPara():
    global ran
    CSParaDataset = []
    for loopcount in range(7, 7 + SFNum):
        ran = TPLevelNum
        for loopcount2 in range(ran):
            CSParaDataset.append(
                'data/nonpower/CS/Parameters/SF' + str(loopcount) + 'TP' + str(loopcount2) + '/data.csv')
    return CSParaDataset


def initCSNonPara():
    CSNonParaDataset = []
    for loopcount in range(2, 2+GWNum):
        for loopcount2 in range(JXNum):
            CSNonParaDataset.append(
                'data/nonpower/CS/NonParameters/GW' + str(loopcount) + 'JX' + str(loopcount2) + '/data.csv')
    return CSNonParaDataset

def initNSNonPara():
    NSNonParaDataset = []
    for loopcount in range(2, 2+GWNum):
        for loopcount2 in range(JXNum):
            NSNonParaDataset.append(
                'data/nonpower/NS/NonParameters/GW' + str(loopcount) + 'JX' + str(loopcount2) + '/data.csv')
    return NSNonParaDataset

def initCSOPRNonPara():
    CSOPRNonParaDataset = []
    for loopcount in range(2, 2+GWNum):
        for loopcount2 in range(JXNum):
            CSOPRNonParaDataset.append(
                'data/nonpower/CS/OPRNonParameters/GW' + str(loopcount) + 'JX' + str(loopcount2) + '/data.csv')
    return CSOPRNonParaDataset

def initEC():
    ECJXNumDataset = []
    for loopcount in range(3 + 1):
        ECJXNumDataset.append('data/power/' + str(loopcount) + '/power.csv')
    return ECJXNumDataset

def initOriginalEC():
    ECJXNumDataset = []
    for loopcount in range(3 + 1):
        ECJXNumDataset.append('data/power/' + str(loopcount) + '/originalpower.csv')
    return ECJXNumDataset


def LoadCSPDR(dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(2, 0), unpack=True)
    return x, y


def loadCSThroughput(dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(2, 3), unpack=True)
    return x, y


def loadNSThroughput(dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(0, 1), unpack=True)
    return x, y


def loadEC(dataset):
    (x, voltage, current) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(0, 3, 10), unpack=True)
    y = voltage * current
    return x, y
