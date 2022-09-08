from matplotlib import pyplot as plt

#定义函数来显示柱子上的数值http://t.csdn.cn/49oKH
def autolabel(rects):
    for rect in rects:
        height = rect.get_height()
        plt.text(rect.get_x()+rect.get_width()/2.-0.03, 1.01*height, '%s' % int(height), size=10, family="Times new roman")
