import numpy as np

GWNumMax = 6
GWNumMin = 2
GWNum = GWNumMax - GWNumMin + 1
EDLevel = 4

SNRStackLevel = 5

RealEDNum = 4
EDNumMax = 8
EDNumMin = 2
MaxEDNum = 8


def initSNRStack():
    SNRStackDataset = []
    for loopcount in range(0, SNRStackLevel):
        SNRStackDataset.append(
            'data/EELoRa/SNRStack/Inter' + str(loopcount) + '/data.csv')
    return SNRStackDataset


def initNonSNRStack():
    NonSNRStackDataset = []
    for loopcount in range(2, 2 + GWNum):
        for loopcount2 in range(EDLevel):
            NonSNRStackDataset.append(
                'data/EELoRa/NonSNRStack/GW' + str(loopcount) + 'ED' + str(loopcount2) + '/data.csv')
    return NonSNRStackDataset


def initDyLoRa():
    DyLoRaDataset = []
    for loopcount in range(2, 2 + GWNum):
        for loopcount2 in range(EDLevel):
            DyLoRaDataset.append(
                'data/DyLoRa/GW' + str(loopcount) + 'ED' + str(loopcount2) + '/data.csv')
    return DyLoRaDataset


def initLoRaWAN():
    LoRaWANDataset = []
    for loopcount in range(2, 2 + GWNum):
        for loopcount2 in range(EDLevel):
            LoRaWANDataset.append(
                'data/LoRaWAN/GW' + str(loopcount) + 'ED' + str(loopcount2) + '/data.csv')
    return LoRaWANDataset


def loadEDFlag(dataset):
    x = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=1, unpack=True)
    return x


def loadEDjInstantEE(j, dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(0, 2 + j), unpack=True)
    return x, y


def loadEDjFinalInstantEE(j, dataset):
    x = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=2 + j, unpack=True)
    x = x[len(x) - 1]
    return x


def loadEDjAverageEE(j, dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(0, 5 + MaxEDNum + 7 + 8 * j), unpack=True)
    return x, y


def loadEDjFinalAverageEE(j, dataset):
    x = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=5 + MaxEDNum + 7 + 8 * j, unpack=True)
    x = x[len(x) - 1]
    return x


def loadMinEE(dataset):
    x = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=MaxEDNum + 2, unpack=True)
    x = x[len(x) - 1]
    return x


def loadFairIndex(dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(0, MaxEDNum + 3), unpack=True)
    return x, y


def loadEDjFinalAveragePDR(j, dataset):
    x = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=5 + MaxEDNum + 3 + 8 * j, unpack=True)
    x = x[len(x) - 1]
    return x


def loadEDjFinalSF(j, dataset):
    x = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=5 + MaxEDNum + 0 + 8 * j, unpack=True)
    x = x[len(x) - 1]
    return x


def loadEDjFinalTP(j, dataset):
    x = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=5 + MaxEDNum + 1 + 8 * j, unpack=True)
    x = x[len(x) - 1]
    return x
