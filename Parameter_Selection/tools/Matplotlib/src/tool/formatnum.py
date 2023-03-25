# Scientific counting method
from matplotlib.ticker import FormatStrFormatter


def formatnum(x, pos):
    return '$%.1f$x$10^{6}$' % (x / 1e6)

# Percentage counting method
def to_percent(temp, position):
    return '%1.0f' % (100 * temp) + '%'

def two_decimal_formatter():
    return FormatStrFormatter('%.2f')
