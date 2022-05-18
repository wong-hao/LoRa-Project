from src.effectiveness.drawThroughout import drawInstantThroughput
from src.effectiveness.drawThroughout import drawEffThroughput
from src.superiority.JX.drawPDR import drawInstantPDR
from src.superiority.GW.drawPDR import drawSupGWPDR
from src.superiority.JX.drawAckEC import drawEC
from src.principle.drawPower import drawPower
from src.principle.drawSubplot import drawSubplot


def main():

    drawInstantPDR()
    drawInstantThroughput()

    drawSupGWPDR()
    drawPower()
    # drawSoft()
    drawEC()
    drawSubplot()

    drawEffThroughput()


if __name__ == '__main__':
    main()
