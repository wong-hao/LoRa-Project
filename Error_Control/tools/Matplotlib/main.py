from src.effectiveness.Parameter.drawThroughout import drawInstantThroughput
from src.effectiveness.Parameter.drawThroughout import drawEffThroughput
from src.superiority.JXNum.drawPDR import drawInstantPDR
from src.superiority.GWNum.drawPDR import drawSupCSPDR
from src.superiority.JXNum.drawAckEC import drawEC
from src.principle.drawPower import drawPower
from src.principle.drawSubplot import drawSubplot


def main():

    drawInstantPDR()
    drawInstantThroughput()

    drawSupCSPDR()
    drawPower()
    # drawSoft()
    drawEC()
    drawSubplot()

    drawEffThroughput()


if __name__ == '__main__':
    main()
