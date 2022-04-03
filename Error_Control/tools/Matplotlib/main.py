from src.effectiveness.drawData import drawData
from src.effectiveness.drawEC import drawEC
from src.principle.drawPower import drawPower
from src.principle.drawSoft import drawSoft
from src.principle.drawSubplot import drawSubplot


def main():
    drawData()
    drawPower()
    drawSoft()
    drawEC()
    drawSubplot()


if __name__ == '__main__':
    main()
