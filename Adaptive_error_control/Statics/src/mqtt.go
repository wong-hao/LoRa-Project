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
	MQTT "github.com/eclipse/paho.mqtt.golang"
	"os"
)

const (
	QOS           = 0
	SERVERADDRESS = "tcp://106.14.134.224:1883" //Aliyun

	WRITETOLOG  = true  // If true then received messages will be written to the console
	WRITETODISK = false // If true then received messages will be written to the file below

	OUTPUTFILE = "/binds/receivedMessages.txt"

	USERNAME = "admin"
	PASSWORD = "admin"

	N = 4 //Num of GW
	M = 1 //Num of ED
)

var (
	TOPICRak811ABP   = "application/1/device/53232c5e6c936483/event/up" //Rak811ABP
	TOPICRak811OTAA  = "application/2/device/d930ade299582ab5/event/up" //Rak811OTAA
	TOPICRak4200ABP  = "application/5/device/c0e4ecf4cd399d55/event/up" //Rak4200ABP
	TOPICRak4200OTAA = "application/8/device/3de06c3b2b86702a/event/up" //Rak4200OTAA
	TOPICDraginoABP  = "application/6/device/3bc1efb6e719cc2c/event/up" //DraginoABP
	TOPICDraginoABP2 = "application/6/device/3bc1efb6e719cc2d/event/up" //DraginoABP
	TOPICDraginoOTAA = "application/7/device/8bec4cec640c7c2a/event/up" //DraginoOTAA

	TOPIC    = [...]string{TOPICDraginoABP, TOPICDraginoABP2, TOPICDraginoOTAA, TOPICRak811ABP, TOPICRak811OTAA, TOPICRak4200ABP, TOPICRak4200OTAA}
	CLIENTID = [...]string{"0"}

	opts = [M]*MQTT.ClientOptions{} //mqtt option array
	c    = [M]MQTT.Client{}         //mqtt client array

	ED int //ED flag

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
	Applicationid     string `json:"applicationID"`
	Applicationname   string `json:"applicationName"`
	Devicename        string `json:"deviceName"`
	Deviceprofilename string `json:"deviceProfileName"`
	Deviceprofileid   string `json:"deviceProfileID"`
	Deveui            string `json:"devEUI"`
	Rxinfo            []struct {
		Gatewayid string    `json:"gatewayID"`
		Uplinkid  string    `json:"uplinkID"`
		Name      string    `json:"name"`
		Time      time.Time `json:"time"`
		Rssi      float64   `json:"rssi"`
		Lorasnr   float64   `json:"loRaSNR"`
		Location  struct {
			Latitude  float64 `json:"latitude"`
			Longitude float64 `json:"longitude"`
			Altitude  float64 `json:"altitude"`
		} `json:"location"`
	} `json:"rxInfo"`
	Txinfo struct {
		Frequency int `json:"frequency"`
		Dr        int `json:"dr"`
	} `json:"txInfo"`
	Adr    bool   `json:"adr"`
	Fcnt   int    `json:"fCnt"`
	Fport  int    `json:"fPort"`
	Data   string `json:"data"`
	Object struct {
		Analoginput struct {
			Num3 int `json:"3"`
			Num4 int `json:"4"`
		} `json:"analogInput"`
		Humiditysensor struct {
			Num2 int `json:"2"`
		} `json:"humiditySensor"`
		Temperaturesensor struct {
			Num1 int `json:"1"`
		} `json:"temperatureSensor"`
	} `json:"object"`
}

//define a function for the default message handler
var f MQTT.MessageHandler = func(client MQTT.Client, msg MQTT.Message) {

	//Get ED flag from ClientID
	OptionsReader := client.OptionsReader()
	ClientID := OptionsReader.ClientID()
	ED, _ = strconv.Atoi(ClientID)

	up := UP{}
	if err := json.Unmarshal(msg.Payload(), &up); err != nil {
		fmt.Printf("Message could not be parsed (%s): %s", msg.Payload(), err)
	}

	//fmt.Printf("TOPIC: %s\n", msg.Topic())
	fmt.Printf("MSG: %s\n", msg.Payload())

	DataSlice = append(DataSlice, reflect.ValueOf(up).FieldByName("Data").String())
	getThroughout(DataSlice)
	//UplinkFcntHistorySlice = append(UplinkFcntHistorySlice, int(reflect.ValueOf(up).FieldByName("Fcnt").Int()))
	//getPER(UplinkFcntHistorySlice)

	fmt.Printf("INFO: [up] Program total time use in %f ms\n", 1000*time.Now().Sub(StartTime).Seconds())
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

	//create ClientOptions struct setting the broker address, clientid, turn
	//off trace output and set the default message handler
	for i := 0; i < M; i++ {
		opts[i] = MQTT.NewClientOptions().AddBroker(SERVERADDRESS).SetUsername(USERNAME).SetPassword(PASSWORD)
		opts[i].SetClientID(CLIENTID[i])
		opts[i].SetDefaultPublishHandler(f)
		opts[i].OnConnect = connectHandler
		opts[i].OnConnectionLost = connectLostHandler
		opts[i].ConnectRetry = true
		opts[i].AutoReconnect = true
	}

	//create and start clients using the above ClientOptions
	for i := 0; i < M; i++ {
		c[i] = MQTT.NewClient(opts[i])
		if token := c[i].Connect(); token.Wait() && token.Error() != nil {
			panic(token.Error())
		}
		sub(c[i])
	}

	for i := 0; i < M; i++ {
		exit(c[i])
	}

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

	OptionsReader := client.OptionsReader()
	ClientID := OptionsReader.ClientID()
	ED, _ = strconv.Atoi(ClientID)

	if token := client.Subscribe(TOPIC[ED], QOS, nil); token.Wait() && token.Error() != nil {
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

	if len(str) == 4 {
		//fmt.Println(str)
		err1 := WriterCsv.Write(str)
		if err1 != nil {
			log.Println("WriterCsv写入文件失败")
		}
		WriterCsv.Flush() //刷新，不刷新是无法写入的
		str = str[0:0]
	}
}
