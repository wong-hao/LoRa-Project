//https://www.eclipse.org/paho/index.php?page=clients/golang/index.php
//https://github.com/eclipse/paho.mqtt.golang/issues/507
//https://www.emqx.cn/blog/how-to-use-mqtt-in-golang

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
	SERVERADDRESS = "tcp://106.14.134.224:1883" //Aliyun

	WRITETOLOG  = true  // If true then received messages will be written to the console
	WRITETODISK = false // If true then received messages will be written to the file below

	OUTPUTFILE = "/binds/receivedMessages.txt"

	USERNAME = "admin"
	PASSWORD = "admin"

	N = 4 //Num of GW
	M = 1 //Num of ED

	Tinterval = 20 //Transmission interval

)

var (
	TOPICDraginoABP  = "application/1/device/3bc1efb6e719cc2c/event/up" //DraginoABP
	TOPICDraginoABP2 = "application/1/device/3bc1efb6e719cc2d/event/up" //DraginoABP
	TOPICRak811ABP   = "application/1/device/53232c5e6c936483/event/up" //Rak811ABP
	TOPICRak811OTAA  = "application/2/device/d930ade299582ab5/event/up" //Rak811OTAA
	TOPICRak4200ABP  = "application/5/device/c0e4ecf4cd399d55/event/up" //Rak4200ABP
	TOPICRak4200OTAA = "application/8/device/3de06c3b2b86702a/event/up" //Rak4200OTAA
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

	//Get current time
	SnapshotTime = time.Now()

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

	fmt.Printf("FCNT: %d\n", int(reflect.ValueOf(up).FieldByName("Fcnt").Int())) //Only for debug
	fmt.Printf("INFO: [up] Program total time use in %f ms\n", 1000*SnapshotTime.Sub(InitTime).Seconds())
	fmt.Printf("GoodputData: %f Byte\n", GoodputData)
	fmt.Printf("Goodput: %f kbps\n", Goodput)
	fmt.Printf("ThroughputData: %f Byte\n", ThroughputData)
	fmt.Printf("Throughput: %f kbps\n\n", Throughput)

	logData(1000*SnapshotTime.Sub(InitTime).Seconds(), Throughput, reflect.ValueOf(up).FieldByName("Data").String())
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

	fmt.Printf("ED num: %d, GW num: %d\n", M, N)

	for i := 0; i < M; i++ {
		exit(c[i])
		unsub(c[i])
	}

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

func pub(client MQTT.Client) {
	//Publish 5 messages to /go-mqtt/sample at qos 1 and wait for the receipt
	//from the server after sending each message

	OptionsReader := client.OptionsReader()
	ClientID := OptionsReader.ClientID()
	ED, _ = strconv.Atoi(ClientID)

	for i := 0; i < 5; i++ {
		text := " {\"applicationID\":\"6\",\"applicationName\":\"DraginoABP\",\"deviceName\":\"DraginoShield\",\"deviceProfileName\":\"DraginoABP\",\"deviceProfileID\":\"242002f1-46df-4feb-83c6-ec4b9aea138f\",\"devEUI\":\"3bc1efb6e719cc2c\",\"rxInfo\":[{\"gatewayID\":\"0016c001ff10d3f7\",\"uplinkID\":\"24395e0e-2f51-4a18-ad6c-e9a67a1c7f5c\",\"name\":\"raspberrypi2\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-67,\"loRaSNR\":7,\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}},{\"gatewayID\":\"0016c001ff10d446\",\"uplinkID\":\"b7dd0d5c-1586-4837-aaaa-19a6ef7a5d7c\",\"name\":\"raspberrypi4\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-66,\"loRaSNR\":9.2,\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}},{\"gatewayID\":\"0016c001ff10d42d\",\"uplinkID\":\"86d17570-51d9-47fb-80a3-3cc673bfbd8d\",\"name\":\"raspberrypi3\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-69,\"loRaSNR\":10.8,\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}},{\"gatewayID\":\"0016c001ff10d3f6\",\"uplinkID\":\"5a155a7c-16d0-45d1-bf46-b7c54dddbd58\",\"name\":\"raspberrypi1\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-78,\"loRaSNR\":4.5,\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}}],\"txInfo\":{\"frequency\":486900000,\"dr\":2},\"adr\":false,\"fCnt\":3,\"fPort\":1,\"data\":\"AWcAAAJoAAMCAAAEAgAA\",\"object\":{\"analogInput\":{\"3\":0,\"4\":0},\"humiditySensor\":{\"2\":0},\"temperatureSensor\":{\"1\":0}}}\n"
		token := client.Publish(TOPIC[ED], 0, false, text)
		token.Wait()
		time.Sleep(Tinterval)
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

	Goodput = (GoodputData * 8) / (1000 * SnapshotTime.Sub(InitTime).Seconds())
	Throughput = (ThroughputData * 8) / (1000 * SnapshotTime.Sub(InitTime).Seconds())
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
