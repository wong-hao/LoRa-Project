import math

import numpy as np
n = 3
GWNumMax = 6
GWNumMin = 2
GWNum = GWNumMax - GWNumMin + 1
JXLevel = 4

TPNum = 8
TPLevelNum = 3
SFNum = 6

TotalDataset = GWNum * JXLevel * n
MidDataset = math.floor((TotalDataset-1) / 2)

def initCSPara():
    global ran
    CSParaDataset = []
    for loopcount in range(7, 7 + SFNum):
        ran = TPLevelNum
        for loopcount2 in range(ran):
            for loopcount3 in range(n):
                CSParaDataset.append(
                    'data/nonpower/CS/Parameters/SF' + str(loopcount) + 'TP' + str(loopcount2) + '/data'+str(loopcount3) + '.csv')
    return CSParaDataset

def initCSNonPara():
    CSNonParaDataset = []
    for loopcount in range(2, 2+GWNum):
        for loopcount2 in range(JXLevel):
            for loopcount3 in range(n):
                CSNonParaDataset.append(
                    'data/nonpower/CS/NonParameters/GW' + str(loopcount) + 'JX' + str(loopcount2) + '/data'+str(loopcount3) + '.csv')
    return CSNonParaDataset

def initNSNonPara():
    NSNonParaDataset = []
    for loopcount in range(2, 2+GWNum):
        for loopcount2 in range(JXLevel):
            for loopcount3 in range(n):
                NSNonParaDataset.append(
                    'data/nonpower/NS/NonParameters/GW' + str(loopcount) + 'JX' + str(loopcount2) + '/data'+str(loopcount3) + '.csv')
    return NSNonParaDataset

def initCSOPRNonPara():
    CSOPRNonParaDataset = []
    for loopcount in range(2, 2+GWNum):
        for loopcount2 in range(JXLevel):
            for loopcount3 in range(n):
                CSOPRNonParaDataset.append(
                    'data/nonpower/CS/OPRNonParameters/GW' + str(loopcount) + 'JX' + str(loopcount2) + '/data'+str(loopcount3) + '.csv')
    return CSOPRNonParaDataset

def initEC():
    ECJXLevelDataset = []
    for loopcount in range(3 + 1):
        ECJXLevelDataset.append('data/power/' + str(loopcount) + '/power.csv')
    return ECJXLevelDataset

def initOriginalEC():
    ECJXLevelDataset = []
    for loopcount in range(3 + 1):
        ECJXLevelDataset.append('data/power/' + str(loopcount) + '/originalpower.csv')
    return ECJXLevelDataset


def loadCSPDR(dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(2, 0), unpack=True)
    return x, y

def loadCSFinalPDR(dataset):
    x = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=0, unpack=True)
    x = x[len(x)-1]
    return x

def loadCSThroughput(dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(2, 3), unpack=True)
    return x, y

def loadCSFinalThroughput(dataset):
    x = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=3, unpack=True)
    x = x[len(x)-1]
    return x

def loadNSThroughput(dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(0, 1), unpack=True)
    return x, y


def loadEC(dataset):
    (x, voltage, current) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(0, 3, 10), unpack=True)
    y = voltage * current
    return x, y
