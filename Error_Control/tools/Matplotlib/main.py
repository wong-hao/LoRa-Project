from src.effectiveness.drawThroughout import drawEffThroughput
from src.superiority.drawInstantPDR import drawInstantPDR
from src.superiority.drawAckEC import drawEC
from src.effectiveness.drawInstantThroughout import drawInstantThroughput
from src.principle.drawPower import drawPower
from src.principle.drawSubplot import drawSubplot
from src.superiority.GW.drawPDR import drawSupGWPDR


def main():
    drawInstantPDR()
    drawInstantThroughput()

    drawSupGWPDR()
    drawPower()
    # drawSoft()
    drawEC()
    drawSubplot()
    # drawPM()

    drawEffThroughput()


if __name__ == '__main__':
    main()
