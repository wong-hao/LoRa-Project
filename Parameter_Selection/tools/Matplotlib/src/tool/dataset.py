import numpy as np

SNRStackLevel = 5

RealEDNum = 4
MaxEDNum = 8


def initSNRStack():
    SNRStackDataset = []
    for loopcount in range(0, SNRStackLevel):
        SNRStackDataset.append(
            'data/EELoRa/SNRStack/Inter' + str(loopcount) + '/data.csv')
    return SNRStackDataset

def loadEDjInstantEE(j, dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(0, 2+j), unpack=True)
    return x, y

def loadEDjAverageEE(j, dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(0, 5+MaxEDNum+7+10*j), unpack=True)
    return x, y
