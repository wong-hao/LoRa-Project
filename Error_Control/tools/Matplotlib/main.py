from src.effectiveness.Parameter.drawPDR import drawEffPDR
from src.effectiveness.Parameter.drawThroughout import drawInstantThroughput
from src.effectiveness.Parameter.drawThroughout import drawEffThroughput
from src.superiority.JXNum.drawPDR import drawInstantPDR
from src.superiority.GWNum.drawPDR import drawSupCSPDRMid, drawSupCSPDRLow, drawSupCSPDRHigh
from src.effectiveness.drawAckEC import drawInstantEC, drawAverageEC
from src.principle.drawPower import drawPower
from src.principle.drawSubplot import drawSubplot

def main():

    #drawInstantPDR()
    drawInstantThroughput()

    drawSupCSPDRLow()
    drawSupCSPDRMid()
    drawSupCSPDRHigh()
    drawPower()
    # drawSoft()

    drawInstantEC()
    drawAverageEC()

    drawSubplot()

    drawEffThroughput()
    drawEffPDR()

if __name__ == '__main__':
    main()
