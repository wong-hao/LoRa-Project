import matplotlib.pyplot as plt
import numpy as np

# Stage1 Power figure
figure1 = plt.figure()

x1 = np.loadtxt('power.csv', delimiter=',', usecols=0, unpack=True)
y1 = np.loadtxt('power.csv', delimiter=',', usecols=3, unpack=True)
y2 = np.loadtxt('power.csv', delimiter=',', usecols=2, unpack=True)

fig1, ax1 = plt.subplots()
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

l1, = ax1.plot(x1, y1, color='r')
l2, = ax2.plot(x1, y2, color='b')

plt.legend(handles=[l1, l2, ], labels=['USB Avg Current (mA)', 'Main Avg Voltage (V)'], loc='best')

plt.savefig("power.pdf", format="pdf")  # latex
# plt.savefig("power.csv", format="csv") word
plt.show()

# Stage2 Data figure
x2 = np.loadtxt('Data.csv', delimiter=',', usecols=6, unpack=True)
y3 = np.loadtxt('Data.csv', delimiter=',', usecols=4, unpack=True)
y4 = np.loadtxt('Data.csv', delimiter=',', usecols=7, unpack=True)

figure2 = plt.figure()

fig2, ax3 = plt.subplots()
ax4 = ax3.twinx()

ax3.set_xlim((0, 2400000))
ax3.set_ylim((0, 1))
ax3.set_xlabel('Time (ms)')
ax3.set_ylabel('Packet delivery radio')

ax4.set_xlim((0, 2400000))
ax4.set_ylim((0, 0.1))
ax4.set_ylabel('Throughout(kbp)')

l3, = ax3.plot(x2, y3, color='r')
l4, = ax4.plot(x2, y4, color='b')

plt.legend(handles=[l3, l4, ], labels=['Packet delivery radio', 'Throughout(kbp)'], loc='best')

plt.savefig("Data.pdf", format="pdf")

plt.show()
