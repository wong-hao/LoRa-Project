import math

import numpy as np

n = 3
GWNumMax = 6
GWNumMin = 2
GWNum = GWNumMax - GWNumMin + 1
EDLevel = 6

SNRStackLevel = 5

RealEDNum = 12
EDNumMax = 12
EDNumMin = 2
MaxEDNum = 12
LenofEDinfo = 5

TotalDataset = GWNum * EDLevel * n
MidDataset = math.ceil((TotalDataset-1) / 2)

TxpowerArrayWatt = [950.0, 872.0, 750.0, 603.0, 556.0, 500.0, 470.0, 435.0]
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
            for loopcount3 in range(n):
                NonSNRStackDataset.append(
                    'data/EELoRa/NonSNRStack/GW' + str(loopcount) + 'ED' + str(loopcount2) + '/data'+str(loopcount3) + '.csv')
    return NonSNRStackDataset

def initDyLoRa():
    DyLoRaDataset = []
    for loopcount in range(2, 2 + GWNum):
        for loopcount2 in range(EDLevel):
            for loopcount3 in range(n):
                 DyLoRaDataset.append(
                'data/DyLoRa/GW' + str(loopcount) + 'ED' + str(loopcount2) + '/data'+str(loopcount3) + '.csv')
    return DyLoRaDataset

def initEFLoRa():
    EFLoRaDataset = []
    for loopcount in range(2, 2 + GWNum):
        for loopcount2 in range(EDLevel):
            for loopcount3 in range(n):
                EFLoRaDataset.append(
                    'data/EFLoRa/GW' + str(loopcount) + 'ED' + str(loopcount2) + '/data'+str(loopcount3) + '.csv')
    return EFLoRaDataset

def initLoRaWAN():
    LoRaWANDataset = []
    for loopcount in range(2, 2 + GWNum):
        for loopcount2 in range(EDLevel):
            for loopcount3 in range(n):
                 LoRaWANDataset.append(
                    'data/LoRaWAN/GW' + str(loopcount) + 'ED' + str(loopcount2) + '/data'+str(loopcount3) + '.csv')
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

def loadMinEE(dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(0, MaxEDNum + 2), unpack=True)
    return x, y

def loadFinalMinEE(dataset):
    x = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=MaxEDNum + 2, unpack=True)
    x = x[len(x) - 1]
    return x


def loadFairIndex(dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(0, MaxEDNum + 3), unpack=True)
    return x, y


def loadEDjFinalSF(j, dataset):
    x = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=4 + MaxEDNum + (LenofEDinfo * j + 1), unpack=True)
    x = x[len(x) - 1]
    return x


def loadEDjFinalTP(j, dataset):
    x = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=4 + MaxEDNum + (LenofEDinfo * j + 2), unpack=True)
    x = x[len(x) - 1]
    return x

def loadEDjFinalAveragePRR(j, dataset):
    x = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=4 + MaxEDNum + (LenofEDinfo * j + 4), unpack=True)
    x = x[len(x) - 1]
    return x

def loadEDjAverageEE(j, dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(0, 4 + MaxEDNum + (LenofEDinfo * j + 5)), unpack=True)
    return x, y


def loadEDjFinalAverageEE(j, dataset):
    x = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=4 + MaxEDNum + (LenofEDinfo * j + 5), unpack=True)
    x = x[len(x) - 1]
    return x

