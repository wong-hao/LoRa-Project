import numpy as np

GWNum = 5
JXNum = 3

TPNum = 8
TPLevelNum = 3
TPOption = 1
SFNum = 6

def initCSPara():
    global ran
    CSParaDataset = []
    for loopcount in range(7, 7 + SFNum):
        if TPOption == 0:
            ran = TPNum
        elif TPOption == 1:
            ran = TPLevelNum
        else:
            print(r'TPOption is illegal!')
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
    CSNonParaDataset = []
    for loopcount in range(2, 2+GWNum):
        for loopcount2 in range(JXNum):
            CSNonParaDataset.append(
                'data/nonpower/NS/NonParameters/GW' + str(loopcount) + 'JX' + str(loopcount2) + '/data.csv')
    return CSNonParaDataset

def initCSOPRNonPara():
    CSOPRNonParaDataset = []
    for loopcount in range(2, 2+GWNum):
        for loopcount2 in range(JXNum):
            CSOPRNonParaDataset.append(
                'data/nonpower/CS/OPRNonParameters/GW' + str(loopcount) + 'JX' + str(loopcount2) + '/data.csv')
    return CSOPRNonParaDataset

def initECJXNum():
    ECJXNumDataset = []
    for loopcount in range(3 + 1):
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
