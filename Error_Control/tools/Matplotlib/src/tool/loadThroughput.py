import numpy as np


def loadGWTHroughput(dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(6, 7), unpack=True)
    return x, y


def loadNSTHroughput(dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(0, 1), unpack=True)
    return x, y
