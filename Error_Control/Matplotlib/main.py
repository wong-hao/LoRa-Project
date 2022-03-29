from src.drawData import drawData
from src.drawEnergyEfficiency import drawEnergyEfficiency
from src.drawPower import drawPower
from src.drawSoft import drawSoft
from src.drawSubplot import drawSubplot


def main():
    drawData()
    drawPower()
    drawSoft()
    drawEnergyEfficiency()
    drawSubplot()


if __name__ == '__main__':
    main()
