//https://www.eclipse.org/paho/index.php?page=clients/golang/index.php
//https://github.com/eclipse/paho.mqtt.golang/issues/507
//https://www.emqx.cn/blog/how-to-use-mqtt-in-golang

package src

import (
	"encoding/base64"
	"encoding/csv"
	"encoding/json"
	"fmt"
	"log"
	"strconv"

	//"github.com/shopspring/decimal"
	"os/signal"
	"reflect"

	"syscall"
	"time"

	//import the Paho Go MQTT library
	"os"

	MQTT "github.com/eclipse/paho.mqtt.golang"
)

const (
	//TOPIC         = "ttt"// test
	TOPIC2 = "tttt" // test
	//TOPIC = "application/1/device/53232c5e6c936483/event/up" //Rak811ABP
	//TOPIC         = "application/2/device/d930ade299582ab5/event/up" //Rak811OTAA
	//TOPIC = "application/5/device/c0e4ecf4cd399d55/event/up" //Rak4200ABP
	//TOPIC = "application/8/device/3de06c3b2b86702a/event/up" //Rak4200OTAA
	TOPIC = "application/6/device/3bc1efb6e719cc2c/event/up" //DraginoABP
	//TOPIC         = "application/7/device/8bec4cec640c7c2a/event/up" //DraginoOTAA

	QOS           = 0
	SERVERADDRESS = "tcp://106.14.134.224:1883" //Aliyun

	CLIENTID  = "go_mqtt_client"
	CLIENTID2 = "go_mqtt_client2"

	WRITETOLOG  = true  // If true then received messages will be written to the console
	WRITETODISK = false // If true then received messages will be written to the file below

	OUTPUTFILE = "/binds/receivedMessages.txt"

	USERNAME = "admin"
	PASSWORD = "admin"

	HISTORYCOUNT = 6
)

var (
	num     = 0
	DR      int
	Txpower = maxTxPower

	messageJson       [HISTORYCOUNT]string
	dataArray         [HISTORYCOUNT]string
	uplinkSNRHistory  [HISTORYCOUNT]float64 //TODO: 求多网关平均值
	uplinkFcntHistory [HISTORYCOUNT]int

	ADR_ACK_Req bool

	NbTrans int = 1

	DataSlice              []string
	UplinkFcntHistorySlice []int

	GoodputData float64 //Frame Payload
	// TODO: 这里计算的单个节点的吞吐量，而论文中均是整个网络中共同传输的节点的总吞吐量；论文似乎是以通过CRC校验的计算而非MIC校验
	ThroughputData float64 //PHY Payload (论文应该是以整个PHY Packet，包含metadata等计算），可观察网关PUSH_DATA datagrams sent(不含stat报告)的大小(会随发送内容改变)
	Goodput        float64
	Throughput     float64
	LenofElement   int
	StartTime      = time.Now() // 获取当前时间

	str      []string
	fileName = time.Now().Format("2006-01-02-15-04-05")
	fileType = "-Dataset.csv"
	path     = "./bin/"
	header   = []string{"TotalTime(ms)", "Throughout(kbp)", "data", "time"}
	row      = 0
)

type UP struct {
	Applicationid   string `json:"applicationID"`
	Applicationname string `json:"applicationName"`
	Devicename      string `json:"deviceName"`
	Deveui          string `json:"devEUI"`
	Rxinfo          []struct {
		Gatewayid string    `json:"gatewayID"`
		Uplinkid  string    `json:"uplinkID"`
		Name      string    `json:"name"`
		Time      time.Time `json:"time"`
		Rssi      float64   `json:"rssi"`
		Lorasnr   float64   `json:"loRaSNR"`
		Location  struct {
			Latitude  float64 `json:"latitude"`
			Longitude float64 `json:"longitude"`
			Altitude  int     `json:"altitude"`
		} `json:"location"`
	} `json:"rxInfo"`
	Txinfo struct {
		Frequency int `json:"frequency"`
		Dr        int `json:"dr"`
	} `json:"txInfo"`
	Adr   bool   `json:"adr"`
	Fcnt  int    `json:"fCnt"`
	Fport int    `json:"fPort"`
	Data  string `json:"data"`
}

//define a function for the default message handler
var f MQTT.MessageHandler = func(client MQTT.Client, msg MQTT.Message) {

	up := UP{}
	if err := json.Unmarshal(msg.Payload(), &up); err != nil {
		fmt.Printf("Message could not be parsed (%s): %s", msg.Payload(), err)
	}

	//fmt.Printf("TOPIC: %s\n", msg.Topic())
	//fmt.Printf("MSG: %s\n", msg.Payload())

	if num < HISTORYCOUNT {
		messageJson[num] = string(msg.Payload())

		dataArray[num] = reflect.ValueOf(up).FieldByName("Data").String()

		for _, u := range up.Rxinfo {
			uplinkSNRHistory[num] = u.Lorasnr
		}

		uplinkFcntHistory[num] = int(reflect.ValueOf(up).FieldByName("Fcnt").Int())

	} else {
		for i := 0; i <= HISTORYCOUNT-2; i++ {
			messageJson[i] = messageJson[i+1]
			dataArray[i] = dataArray[i+1]
			uplinkSNRHistory[i] = uplinkSNRHistory[i+1]
			uplinkFcntHistory[i] = uplinkFcntHistory[i+1]
		}

		messageJson[HISTORYCOUNT-1] = string(msg.Payload())

		dataArray[HISTORYCOUNT-1] = reflect.ValueOf(up).FieldByName("Data").String()

		for _, u := range up.Rxinfo {
			uplinkSNRHistory[HISTORYCOUNT-1] = u.Lorasnr
		}

		uplinkFcntHistory[HISTORYCOUNT-1] = int(reflect.ValueOf(up).FieldByName("Fcnt").Int())

		DR = int(reflect.ValueOf(up.Txinfo).FieldByName("Dr").Int())

		ADR_ACK_Req = reflect.ValueOf(up).FieldByName("Adr").Bool()
		if ADR_ACK_Req == true {
			defalutADR(DR, &Txpower, &NbTrans)
			//testADR(num,&Txpower)
		}

	}
	num++

	//fmt.Printf("The number of received message: %d\n",num)
	//fmt.Printf("Received mssage: %v\n" , messageJson)
	fmt.Printf("Uplink Data history: %v\n", dataArray)
	fmt.Printf("Uplink SNR history: %v\n", uplinkSNRHistory)
	fmt.Printf("Uplink Fcnt history: %v\n", uplinkFcntHistory)

	DataSlice = append(DataSlice, reflect.ValueOf(up).FieldByName("Data").String())
	getThroughout(DataSlice)
	fmt.Printf("INFO: [up] Program total time use in %f ms\n", time.Now().Sub(StartTime).Seconds())
	fmt.Printf("GoodputData: %f Byte\n", GoodputData)
	fmt.Printf("Goodput: %f kbps\n", Goodput)
	fmt.Printf("ThroughputData: %f Byte\n", ThroughputData)
	fmt.Printf("Throughput: %f kbps\n\n", Throughput)

	logData(1000*time.Now().Sub(StartTime).Seconds(), Throughput, reflect.ValueOf(up).FieldByName("Data").String())
}

var connectHandler MQTT.OnConnectHandler = func(client MQTT.Client) {
	fmt.Println("Connected")
}

var connectLostHandler MQTT.ConnectionLostHandler = func(client MQTT.Client, err error) {
	fmt.Printf("Connect lost: %v", err)
	client.Disconnect(250)

}

func Paho() {
	//create a ClientOptions struct setting the broker address, clientid, turn
	//off trace output and set the default message handler
	opts := MQTT.NewClientOptions().AddBroker(SERVERADDRESS).SetUsername(USERNAME).SetPassword(PASSWORD)
	opts.SetClientID(CLIENTID)
	opts.SetDefaultPublishHandler(f)
	opts.OnConnect = connectHandler
	opts.OnConnectionLost = connectLostHandler
	opts.ConnectRetry = true
	opts.AutoReconnect = true

	opts2 := MQTT.NewClientOptions().AddBroker(SERVERADDRESS).SetUsername(USERNAME).SetPassword(PASSWORD)
	opts2.SetClientID(CLIENTID2)
	opts2.SetDefaultPublishHandler(f)
	opts2.OnConnect = connectHandler
	opts2.OnConnectionLost = connectLostHandler
	opts2.ConnectRetry = true
	opts2.AutoReconnect = true

	//create and start a client using the above ClientOptions
	c := MQTT.NewClient(opts)
	if token := c.Connect(); token.Wait() && token.Error() != nil {
		panic(token.Error())
	}

	//create and start another client using the above ClientOptions
	c2 := MQTT.NewClient(opts2)
	if token := c2.Connect(); token.Wait() && token.Error() != nil {
		panic(token.Error())
	}

	sub(c)
	sub(c2)

	exit(c)
	exit(c2)
	//unsubscribe from /go-mqtt/sample
	//if token := c.Unsubscribe("application/3/device/53232c5e6c936483/event/#"); token.Wait() && token.Error() != nil {
	//	fmt.Println(token.Error())
	//	os.Exit(1)
	//}

	//c.Disconnect(250)
}

func sub(client MQTT.Client) {
	//subscribe to the topic /go-mqtt/sample and request messages to be delivered
	//at a maximum qos of zero, wait for the receipt to confirm the subscription
	if token := client.Subscribe(TOPIC, QOS, nil); token.Wait() && token.Error() != nil {
		fmt.Println(token.Error())
		os.Exit(1)
	}

}

func exit(clinet MQTT.Client) { //https://github.com/eclipse/paho.mqtt.golang/issues/507#issuecomment-842761501

	// Messages will be delivered asynchronously so we just need to wait for a signal to shutdown
	sig := make(chan os.Signal, 1)
	signal.Notify(sig, os.Interrupt)
	signal.Notify(sig, syscall.SIGTERM)

	<-sig
	fmt.Println("signal caught - exiting")
	clinet.Disconnect(1000)
	fmt.Println("shutdown complete")

}

func getThroughout(DataSlice []string) { //与网关处相同
	GoodputData = 0
	ThroughputData = 0

	for _, j := range DataSlice {
		decodeBytes, err := base64.StdEncoding.DecodeString(j)
		if err != nil {
			log.Fatalln(err)
		}

		LenofElement = len(string(decodeBytes))
		GoodputData = GoodputData + float64(LenofElement)
		ThroughputData = ThroughputData + float64(LenofElement) + 13
	}
	Goodput = (GoodputData * 8) / (1000 * time.Now().Sub(StartTime).Seconds())
	Throughput = (ThroughputData * 8) / (1000 * time.Now().Sub(StartTime).Seconds())
}

func getPER(UplinkFcntHistorySlice []int) float64 { //deprecated: 比网关处的Packet error rate After多了“网关没有全部收到就没有进行纠错”的现象
	var lostPackets int
	var previousFCnt int
	var length float64

	for i, m := range UplinkFcntHistorySlice {
		if i == 0 {
			previousFCnt = m
			continue
		}

		lostPackets += m - previousFCnt - 1 // there is always an expected difference of 1
		previousFCnt = m
	}

	length = float64(UplinkFcntHistorySlice[len(UplinkFcntHistorySlice)-1] - 0 + 1)

	fmt.Printf("UplinkFcntHistory: %v\n\n", UplinkFcntHistorySlice)

	return float64(lostPackets) / length * 100
}

func logData(totaltime float64, throughout float64, data string) {
	if row == 0 {
		fileName = fileName + fileType
		path = path + fileName
	}

	//OpenFile读取文件，不存在时则创建，使用追加模式
	File, err := os.OpenFile(path, os.O_RDWR|os.O_APPEND|os.O_CREATE, 0666)
	if err != nil {
		log.Println("文件打开失败！")
	}
	defer func(File *os.File) {
		err := File.Close()
		if err != nil {

		}
	}(File)

	//创建写入接口
	WriterCsv := csv.NewWriter(File)

	if row == 0 {
		err1 := WriterCsv.Write(header)
		if err1 != nil {
			log.Println("WriterCsv写入文件失败")
		}
	}

	row++

	timeString := strconv.FormatFloat(totaltime, 'f', 0, 64)
	str = append(str, timeString)
	throughoutString := strconv.FormatFloat(throughout, 'f', 6, 64)
	str = append(str, throughoutString)
	str = append(str, data)
	str = append(str, time.Now().Format("2006-01-02T15:04:05Z"))

	if len(str) == 5 {
		//fmt.Println(str)
		err1 := WriterCsv.Write(str)
		if err1 != nil {
			log.Println("WriterCsv写入文件失败")
		}
		WriterCsv.Flush() //刷新，不刷新是无法写入的
		str = str[0:0]
	}
}
