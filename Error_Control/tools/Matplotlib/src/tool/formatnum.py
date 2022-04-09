# Scientific counting method
def formatnum(x, pos):
    return '$%.1f$x$10^{6}$' % (x / 1e6)

def formatnum2(x, pos):
    return '$%.1f$x$10^{-2}$' % (x / 1e-2)

# Percentage counting method
def to_percent(temp, position):
    return '%1.0f' % (100 * temp) + '%'
