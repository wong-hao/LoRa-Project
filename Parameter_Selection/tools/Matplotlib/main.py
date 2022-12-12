from src.effectiveness.SNRStack.drawAverageEE import drawEffAverageEE
from src.principle.drawConvergence import drawConvergence
from src.principle.drawFairness import drawFairness
from src.superiority.drawInstantEE import drawInstantEEED2
from src.superiority.drawMinEE import drawMinEEED2
from src.superiority.drawPDR import drawPDR


def main():
    drawEffAverageEE()
    drawConvergence()
    drawFairness()
    drawMinEEED2()
    drawInstantEEED2()
    drawPDR()

if __name__ == '__main__':
    main()
