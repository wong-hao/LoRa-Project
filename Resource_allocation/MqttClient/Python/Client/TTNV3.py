# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.

import contextlib2
import paho.mqtt.subscribe as subscribe

def print_hi(name):
    # Use a breakpoint in the code line below to debug your script.
    print(f'Hi, {name}')  # Press Ctrl+F8 to toggle the breakpoint.


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    print_hi('PyCharm')
    m = subscribe.simple(topics=['#'], hostname="au1.cloud.thethings.network", port=1883,
                         auth={'username': "rak811abp@ttn", 'password': "NNSXS.CMJKLEQUDSVJLI7WTXXZ4XAY7DU7BETYDQ2HA5Q.ZUCJVBIDGVCXRI6Q26H2EDL56E2UMESJHS6UJMC62WMBDAUC6YPA"}, msg_count=100)
    for a in m:
        print(a.topic)
        print(a.payload)

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
