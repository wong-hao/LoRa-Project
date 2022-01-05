import matplotlib.pyplot as plt
import numpy as np

x = np.loadtxt('classA.csv', delimiter=',', usecols=0, unpack=True)
y1 = np.loadtxt('classA.csv', delimiter=',', usecols=3, unpack=True)
y2 = np.loadtxt('classA.csv', delimiter=',', usecols=2, unpack=True)

fig, ax1 = plt.subplots()
ax2 = ax1.twinx()

# plt.xticks(np.linspace(7, 11, 10))
# plt.yticks(np.linspace(0, 200, 10))

ax1.set_xlim((7, 11))
ax1.set_ylim((0, 200))
ax1.set_xlabel('Time (s)')
ax1.set_ylabel('USB Avg Current (mA)')

ax2.set_xlim((7, 11))
ax2.set_ylim((4, 5.5))
ax2.set_ylabel('Main Avg Voltage (V)')

l1, = ax1.plot(x, y1, color='r')
l2, = ax2.plot(x, y2, color='b')

plt.legend(handles=[l1, l2, ], labels=['USB Avg Current (mA)', 'Main Avg Voltage (V)'], loc='best')
plt.savefig("classA.svg", format="svg")
plt.show()
