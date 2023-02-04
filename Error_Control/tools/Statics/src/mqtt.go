//https://www.eclipse.org/paho/index.php?page=clients/golang/index.php
//https://github.com/eclipse/paho.mqtt.golang/issues/507
//https://www.emqx.com/zh/blog/how-to-use-mqtt-in-golang

package src

import (
	"encoding/base64"
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
	MQTTPort      = "1883"
	SERVERADDRESS = "106.14.134.224" //Aliyun
	MQTTServer    = "tcp://" + SERVERADDRESS + ":" + MQTTPort
	WRITETOLOG    = true  // If true then received messages will be written to the console
	WRITETODISK   = false // If true then received messages will be written to the file below

	OUTPUTFILE = "/binds/receivedMessages.txt"

	USERNAME = "admin"
	PASSWORD = "admin"

	N = 4 //Real number of GW
	M = 8 //Maximal number of ED

	Tinterval = 10 //Transmission interval

)

var (
	devtop1  = "application/1/device/3bc1efb6e719cc2c/event/up"
	devtop2  = "application/1/device/4a659967aafee471/event/up"
	devtop3  = "application/1/device/dcb35cae798148c0/event/up"
	devtop4  = "application/1/device/93fb2867fe31bd72/event/up"
	devtop5  = "application/1/device/1c3f1a2a40c6cd93/event/up"
	devtop6  = "application/1/device/a506893481645dd3/event/up"
	devtop7  = "application/1/device/a506893481645dd4/event/up"
	devtop8  = "application/1/device/a506893481645dd5/event/up"
	devtop9  = "application/1/device/a506893481645dd6/event/up"
	devtop10 = "application/1/device/a506893481645dd7/event/up"

	TOPIC = [...]string{devtop1, devtop2, devtop3, devtop4, devtop5, devtop6, devtop7, devtop8, devtop9, devtop10}

	CLIENTID []string

	opts = [M]*MQTT.ClientOptions{} //mqtt option array
	c    = [M]MQTT.Client{}         //mqtt client array

	num [M]int //num of received message
	ED  int    //ED flag

	fcnt                   [M]int
	DataSlice              [M][]string
	UplinkFcntHistorySlice [M][]int

	Frequency [M]int    //Frequency
	Fport     [M]string //Fport

	HumiditySensor    [M]float64 //Humidity
	TemperatureSensor [M]float64 //Temperature
	Latitude          [M]int     //Latitude
	Longitude         [M]int     //Longitude

	GoodputData [M]float64 //Frame Payload
	// TODO: 这里计算的单个节点的吞吐量，而论文中均是整个网络中共同传输的节点的总吞吐量；论文似乎是以通过CRC校验的计算而非MIC校验
	ThroughputData [M]float64 //PHY Payload (论文应该是以整个PHY Packet，包含metadata等计算），可观察网关PUSH_DATA datagrams sent(不含stat报告)的大小(会随发送内容改变)
	Goodput        [M]float64
	Throughput     [M]float64
	LenofElement   [M]int //Physical Layer byte length
	PER            [M]float64
	PDR            [M]float64
	data           [M]string

	algorithm     = true //选择ADR或设计的算法
	algorithmName string
	SOTA          = false //whether to use OPR work
)

// JSON-to-Go: https://mholt.github.io/json-to-go/， 需要MQTT.fx的已对齐json数据包
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
		GpsLocation struct {
			Num3 struct {
				Altitude  int `json:"altitude"`
				Latitude  int `json:"latitude"`
				Longitude int `json:"longitude"`
			} `json:"3"`
		} `json:"gpsLocation"`
		HumiditySensor struct {
			Num2 float64 `json:"2"`
		} `json:"humiditySensor"`
		TemperatureSensor struct {
			Num1 float64 `json:"1"`
		} `json:"temperatureSensor"`
	} `json:"object"`
}

// define a function for the default message handler
var f MQTT.MessageHandler = func(client MQTT.Client, msg MQTT.Message) {

	//Get current time
	SnapshotTime = time.Now()

	//Get ED flag from ClientID
	OptionsReader := client.OptionsReader()
	ClientID := OptionsReader.ClientID()
	ED, _ = strconv.Atoi(ClientID)

	up := UP{}
	if err := json.Unmarshal(msg.Payload(), &up); err != nil {
		fmt.Printf("Message could not be parsed (%s): %s\n", msg.Payload(), err)
	}

	//fmt.Printf("TOPIC: %s\n", msg.Topic())
	fmt.Printf("MSG: %s\n", msg.Payload())

	//Get frequency
	Frequency[ED] = int(reflect.ValueOf(up.Txinfo).FieldByName("Frequency").Int())

	//Get Object
	HumiditySensor[ED] = reflect.ValueOf(up.Object.TemperatureSensor).FieldByName("Num1").Float()
	TemperatureSensor[ED] = reflect.ValueOf(up.Object.HumiditySensor).FieldByName("Num2").Float()
	Latitude[ED] = int(reflect.ValueOf(up.Object.GpsLocation.Num3).FieldByName("Latitude").Int())
	Longitude[ED] = int(reflect.ValueOf(up.Object.GpsLocation.Num3).FieldByName("Longitude").Int())

	//Get port
	Fport[ED] = strconv.Itoa(int(reflect.ValueOf(up).FieldByName("Fport").Int()))

	//Count received messages
	num[ED]++

	if algorithm == true {
		if SOTA == true {
			algorithmName = "OPR"
		} else {
			algorithmName = "ReLoRaWAN"
		}
	} else {
		algorithmName = "ADR"
	}

	fcnt[ED] = int(reflect.ValueOf(up).FieldByName("Fcnt").Int())
	UplinkFcntHistorySlice[ED] = append(UplinkFcntHistorySlice[ED], fcnt[ED])

	data[ED] = reflect.ValueOf(up).FieldByName("Data").String()
	DataSlice[ED] = append(DataSlice[ED], data[ED])

	printStatistic(ED)
	logData(ED)

	fmt.Printf("The number of received message: %d\n\n", num)

	influxdbWriteAlgorithm(ED, SnapshotTime)

}

var connectHandler MQTT.OnConnectHandler = func(client MQTT.Client) {
	fmt.Println("Connected")
}

var connectLostHandler MQTT.ConnectionLostHandler = func(client MQTT.Client, err error) {
	fmt.Printf("Connect lost: %v", err)
	//client.Disconnect(250)

}

func Paho() {

	fmt.Printf("ED num: %d, GW num: %d\n", M, N)

	if algorithm == true {
		if SOTA == true {
			fmt.Printf("OPR On!\n")
		} else {
			fmt.Printf("ReLoRaWAN On!\n")
		}
	} else {
		fmt.Printf("ADR On!\n")
	}

	for i := 0; i < M; i++ {
		opts[i] = MQTT.NewClientOptions().AddBroker(MQTTServer).SetUsername(USERNAME).SetPassword(PASSWORD)
		CLIENTID = append(CLIENTID, strconv.Itoa(i))
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
		unsub(c[i])
	}
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

func unsub(client MQTT.Client) {
	//unsubscribe from /go-mqtt/sample

	OptionsReader := client.OptionsReader()
	ClientID := OptionsReader.ClientID()
	ED, _ = strconv.Atoi(ClientID)

	if token := client.Unsubscribe(TOPIC[ED]); token.Wait() && token.Error() != nil {
		fmt.Println(token.Error())
		os.Exit(1)
	}

	client.Disconnect(250)

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

func getThroughput(ED int) { //虽然网关吞吐数据量相同，但因为有空中传输时间的差距故吞吐量并不完全相同
	GoodputData[ED] = 0
	ThroughputData[ED] = 0

	for _, j := range DataSlice[ED] {
		decodeBytes, err := base64.StdEncoding.DecodeString(j)
		if err != nil {

			log.Fatalln(err)
		}

		LenofElement[ED] = len(string(decodeBytes))
		GoodputData[ED] = GoodputData[ED] + float64(LenofElement[ED])
		ThroughputData[ED] = ThroughputData[ED] + float64(LenofElement[ED]) + 13
	}

	Goodput[ED] = (GoodputData[ED] * 8) / (1000 * SnapshotTime.Sub(InitTime).Seconds())
	Throughput[ED] = (ThroughputData[ED] * 8) / (1000 * SnapshotTime.Sub(InitTime).Seconds())

	fmt.Printf("GoodputData: %v Byte\n", GoodputData)
	fmt.Printf("ThroughputData: %v Byte\n", ThroughputData)
	fmt.Printf("Goodput: %v kbps\n", Goodput)
	fmt.Printf("Throughput: %v kbps\n", Throughput)
}

func getPER(ED int) { //https://github.com/brocaar/chirpstack-network-server/blob/4e7fdb348b5d465c8faacbf6a1f6f5fabea88066/internal/adr/default.go#L137
	//ATTENTION: 与网关处的Packet error ratio“相比，会出现网关没有全部收到正确数据包，就没有将其转发给NS，导致计算公式中的间隔出现错误，而这是无法解释的（吞吐数据量同理），所以这个指标最好不用于有效性的解释而是优越性
	var lostPackets int
	var previousFCnt int
	var length float64

	for i, m := range UplinkFcntHistorySlice[ED] {
		if i == 0 {
			previousFCnt = m
			continue
		}

		lostPackets += m - previousFCnt - 1 // there is always an expected difference of 1
		previousFCnt = m
	}

	length = float64(UplinkFcntHistorySlice[ED][len(UplinkFcntHistorySlice[ED])-1] - 0 + 1)

	PER[ED] = float64(lostPackets) / length
	PDR[ED] = 1 - PER[ED]

	fmt.Printf("Packet error ratio: %v\n", PER)
	fmt.Printf("Packet delivery ratio: %v\n", PDR)
}
