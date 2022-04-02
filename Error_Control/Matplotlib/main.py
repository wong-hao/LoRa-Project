from src.drawData import drawData
from src.drawEC import drawEC
from src.drawPower import drawPower
from src.drawSoft import drawSoft
from src.drawSubplot import drawSubplot


def main():
    drawData()
    drawPower()
    drawSoft()
    drawEC()
    drawSubplot()


if __name__ == '__main__':
    main()
