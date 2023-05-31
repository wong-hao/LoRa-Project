kilo = 1000

# Percentage counting method
def to_percent(temp, position):
    #return '%1.0f' % (100 * temp) + '%'
    return '%1.0f' % (100 * temp)

def one():
    return '%.6f'

def Millisecond2Second(lst):
    for i in range(len(lst)):
        lst[i] = lst[i] / kilo
        lst[i] = int(lst[i])
    return lst

def add_zero_point_one_to_list(lst):
    return [x + 0.1 for x in lst]

def add_zero_point_zero_five_to_list(lst):
    return [x + 0.1 for x in lst]