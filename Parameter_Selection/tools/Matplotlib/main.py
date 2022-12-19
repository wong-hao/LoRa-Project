from src.effectiveness.SNRStack.drawAverageEE import drawEffAverageEE
from src.principle.drawConvergence import drawConvergence
from src.principle.drawFairness import drawFairness
from src.superiority.drawAverageEE import drawSupAverageEE
from src.superiority.drawInstantEE import drawInstantEEED2
from src.superiority.drawMinEE import drawMinEEED2, drawMinEEGW2
from src.superiority.drawPDR import drawPDR
from src.superiority.drawParameters import drawAssignedSF, drawAssignedTP


def main():
    drawEffAverageEE()
    drawSupAverageEE()
    drawConvergence()
    drawFairness()
    drawMinEEED2()
    drawMinEEGW2()
    drawInstantEEED2()
    drawPDR()
    drawAssignedSF()
    drawAssignedTP()

if __name__ == '__main__':
    main()
