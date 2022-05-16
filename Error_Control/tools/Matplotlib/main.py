from src.effectiveness.drawEffectiveness import drawEffectiveness
from src.superiority.drawPDR import drawPDR
from src.superiority.drawAckEC import drawEC
from src.effectiveness.drawThroughout import drawThroughput
from src.principle.drawPower import drawPower
from src.principle.drawSoft import drawSoft
from src.principle.drawSubplot import drawSubplot
from src.superiority.drawPerformance import drawPM


def main():
    drawPDR()
    drawThroughput()
    drawPower()
    # drawSoft()
    drawEC()
    drawSubplot()
    # drawPM()

    drawEffectiveness()


if __name__ == '__main__':
    main()
