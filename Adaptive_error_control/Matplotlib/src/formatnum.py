def formatnum(x, pos):
    return '$%.1f$x$10^{6}$' % (x / 1000000)


def to_percent(temp, position):
    return '%1.0f' % (100 * temp) + '%'
