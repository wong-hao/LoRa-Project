import numpy as np


def loadPDR(dataset):
    (x, y) = np.loadtxt(dataset, skiprows=1, delimiter=',', usecols=(6, 4), unpack=True)
    return x, y
