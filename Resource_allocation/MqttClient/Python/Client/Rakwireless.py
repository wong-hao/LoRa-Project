#https://downloads.rakwireless.com.cn/LoRa%E5%95%86%E4%B8%9A%E5%8C%96%E7%BD%91%E5%85%B3/%E5%95%86%E4%B8%9A%E7%BD%91%E5%85%B3%E8%BF%9E%E6%8E%A5%E7%AC%AC%E4%B8%89%E6%96%B9%E6%9C%8D%E5%8A%A1%E5%99%A8/

#!/usr/bin/env python
import json
import base64
import paho.mqtt.client as mqtt
from datetime import datetime

# mqtt 服务器 IP
mqtt_ip = '111.230.247.253' # mqtt 服务器端口
mqtt_port = 1883
# mqtt 用户名
mqtt_username = 'rakwireless' # mqtt 密码
mqtt_password = 'rakwireless.com' # mqtt 订阅 topic。该 topic 可以订阅所有节点信息
mqtt_rx_topic = 'application/+/device/+/rx'

# 将字符串转换为 16 进制
def str_to_hex(s):
    return r"\x"+r'\x'.join([hex(ord(c)).replace('0x', '') for c in s])

# 一旦订阅到消息，回调此方法
def on_message(mqttc, obj, msg):
    on_print_rak_node_info(msg.payload)

# 打印订阅到的节点信息
def on_print_node_rx_info(json_rx):
    try:
        devEUI = json_rx['devEUI']
        applicationID = json_rx['applicationID']
        applicationName = json_rx['applicationName']
        deviceName = json_rx['deviceName']
        timestamp = json_rx['timestamp']
        fCnt = json_rx['fCnt']
        fPort = json_rx['fPort']
        data = json_rx['data']
        data_hex = str_to_hex(base64.b64decode(data).decode("utf-8"))

        # 将时间戳转换为本地时间

        str_local_time = datetime.fromtimestamp(timestamp)

        print('---------------- devEUI:[%s] rxpk info -------------------' % devEUI)
        print('+\t applicationName:\t%s' % applicationName)
        print('+\t applicationID:\t\t%s' % applicationID)
        print('+\t deviceName:\t\t%s' % deviceName)
        print('+\t datetime:\t\t%s' % str_local_time)
        print('+\t fCnt:\t\t\t%d' % fCnt)
        print('+\t fPort:\t\t\t%d' % fPort)
        print('+\t data:\t\t\t%s' % data)
        print('+\t data_hex:\t\t%s' % data_hex)
        print('----------------------------------------------------------')
    except Exception as e:
        print(e)
    finally:
        pass

# 订阅到节点的数据之后，向节点发送“Hello rak”字符串
def on_print_rak_node_info(payload):
    json_str = payload.decode()
    try:
        json_rx = json.loads(json_str)
        on_print_node_rx_info(json_rx)

        dev_eui = json_rx['devEUI']
        app_id = json_rx['applicationID']

        # 商业网关默认的 tx topic
        tx_topic = 'application/%s/device/%s/tx' % (app_id, dev_eui)
        str_hello = "Hello Rak"

        tx_msg = '{"confirmed":true,"fPort":10,"data":"%s" }' %str(base64.b64encode(str_hello.encode("utf-8")), "utf-8")

        # 发布消息

        mqttc.publish(tx_topic, tx_msg, qos=0, retain=False)
        print('Send \'Hello rak\' to node %s' % dev_eui)
    except Exception as e:
        raise e
    finally:
        pass

mqttc = mqtt.Client()
mqttc.on_message = on_message
# 如果没有用户名和密码，请注释改行
mqttc.username_pw_set(mqtt_username, password=mqtt_password)
# 连接 mqtt broker，心跳时间为 60s
mqttc.connect(mqtt_ip, mqtt_port, 60)
mqttc.subscribe(mqtt_rx_topic, 0)
mqttc.loop_forever()