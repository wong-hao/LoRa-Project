from src.effectiveness.SNRStack.drawAverageEE import drawEffAverageEE
from src.principle.drawConvergence import drawConvergence
from src.principle.drawFairness import drawFairness
from src.principle.drawMinEE import drawMinEE
from src.superiority.drawAverageEE import drawSupAverageEE
from src.superiority.drawInstantEE import drawInstantEEED4, drawInstantEEGW2, drawInstantEEGW4, drawInstantEEGW6, \
    drawInstantEEED8, drawInstantEEED12
from src.superiority.drawMinEE import drawMinEEED4, drawMinEEGW2, drawMinEEGW4, drawMinEEGW6, drawMinEEED4, \
    drawMinEEED8, drawMinEEED12
from src.superiority.drawPRR import drawPRR
from src.superiority.drawParameters import drawAssignedSF, drawAssignedTP


def main():
    drawEffAverageEE()
    drawSupAverageEE()
    drawConvergence()
    drawFairness()
    drawMinEE()

    drawMinEEED4()
    drawMinEEED8()
    drawMinEEED12()
    #drawMinEEGW2()
    #drawMinEEGW4()
    #drawMinEEGW6()

    #WdrawInstantEEED4()
    #rawInstantEEED8()
    #drawInstantEEED12()
    #drawInstantEEED8()
    #drawInstantEEGW2()
    #drawInstantEEGW4()
    #drawInstantEEGW6()
    drawPRR()
    drawAssignedSF()
    drawAssignedTP()


if __name__ == '__main__':
    main()
