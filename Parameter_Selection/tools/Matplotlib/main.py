from src.effectiveness.SNRStack.drawAverageEE import drawEffAverageEE
from src.principle.drawConvergence import drawConvergence
from src.principle.drawFairness import drawFairness


def main():
    drawEffAverageEE()
    drawConvergence()
    drawFairness()

if __name__ == '__main__':
    main()
