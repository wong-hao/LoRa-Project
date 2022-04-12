from src.superiority.drawPDR import drawPDR
from src.effectiveness.drawEC import drawEC
from src.effectiveness.drawThroughout import drawThroughput
from src.principle.drawPower import drawPower
from src.principle.drawSoft import drawSoft
from src.principle.drawSubplot import drawSubplot


def main():
    drawPDR()
    drawThroughput()
    drawPower()
    drawSoft()
    drawEC()
    drawSubplot()


if __name__ == '__main__':
    main()
