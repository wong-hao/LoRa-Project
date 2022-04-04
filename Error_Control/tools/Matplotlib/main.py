from src.effectiveness.drawPDR import drawPDR
from src.effectiveness.drawEC import drawEC
from src.effectiveness.drawThroughout import drawThroughout
from src.principle.drawPower import drawPower
from src.principle.drawSoft import drawSoft
from src.principle.drawSubplot import drawSubplot


def main():
    drawPDR()
    drawThroughout()
    drawPower()
    drawSoft()
    drawEC()
    drawSubplot()


if __name__ == '__main__':
    main()
