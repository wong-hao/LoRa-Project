from src.effectiveness.Parameter.drawPDR import drawEffPDR
from src.effectiveness.Parameter.drawThroughout import drawEffThroughput
from src.superiority.JXLevel.drawPDR import drawInstantPDR
from src.superiority.GWNum.drawPDR import drawSupCSPDRJX2, drawSupCSPDRJX4, drawSupCSPDRJX6, drawSupCSPDRJX8
from src.effectiveness.drawAckEC import drawInstantEC, drawAverageEC
from src.principle.drawThroughout import drawInstantThroughput
from src.principle.drawPower import drawPower
from src.principle.drawSubplot import drawSubplot

def main():

    #drawInstantPDR()
    drawInstantThroughput()

    drawSupCSPDRJX2()
    drawSupCSPDRJX4()
    drawSupCSPDRJX6()
    drawSupCSPDRJX8()
    drawPower()
    # drawSoft()

    drawInstantEC()
    drawAverageEC()

    drawSubplot()

    drawEffThroughput()
    drawEffPDR()

if __name__ == '__main__':
    main()
