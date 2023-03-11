//https://www.eclipse.org/paho/index.php?page=clients/golang/index.php
//https://github.com/eclipse/paho.mqtt.golang/issues/507
//https://www.emqx.com/zh/blog/how-to-use-mqtt-in-golang

package src

import (
	"encoding/base64"
	"encoding/json"
	"fmt"
	"log"
	"math/rand"
	"reflect"
	"strconv"

	//"github.com/shopspring/decimal"
	"os/signal"
	"syscall"
	"time"

	//import the Paho Go MQTT library
	"os"

	MQTT "github.com/eclipse/paho.mqtt.golang"
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

	HISTORYCOUNT = 5               //Recent SNR history num
	N            = 1               //Real number of GW
	M            = 12              //Maximal number of ED (Do not change unless add more device)
	RealMNum     = 4               //Real number of ED
	Tinterval    = 10              //Transmission interval
	Lambda       = 1.0 / Tinterval // Arrival rate (packet/s)

	MAXRuntime = 1800000 //Total runtime of algorithm
)

var (
	devtop1  = "application/1/device/3bc1efb6e719cc2c/event/up"
	devtop2  = "application/1/device/4a659967aafee471/event/up"
	devtop3  = "application/1/device/dcb35cae798148c0/event/up"
	devtop4  = "application/1/device/93fb2867fe31bd72/event/up"
	devtop5  = "application/1/device/1c3f1a2a40c6cd93/event/up"
	devtop6  = "application/1/device/a506893481645dd3/event/up"
	devtop7  = "application/1/device/4bc0e966236b8a35/event/up"
	devtop8  = "application/1/device/6f6bc87b6d5762c1/event/up"
	devtop9  = "application/1/device/c2674e40266c1555/event/up"
	devtop10 = "application/1/device/724ab434d984f3cc/event/up"
	devtop11 = "application/1/device/cb0a13fab5eb1d77/event/up"
	devtop12 = "application/1/device/8163ad2a34af4626/event/up"

	TOPIC = [...]string{devtop1, devtop2, devtop3, devtop4, devtop5, devtop6, devtop7, devtop8, devtop9, devtop10, devtop11, devtop12}

	CLIENTID []string

	opts = [M]*MQTT.ClientOptions{} //mqtt option array
	c    = [M]MQTT.Client{}         //mqtt client array

	num [M]int //num of received message
	ED  int    //ED flag

	fcnt                   [M]int
	UplinkFcntHistorySlice [M][]int

	uplinkSNRHistory    [M][N][]float64 //Recent HISTORYCOUNT SNR history
	uplinkSNRHistoryLen [M]float64      //When the network is bad, it is possible that there are only less than N SNR records, resulting in an error in average calculation

	adr [M]bool //ACK bit

	Lpayload               [M]float64 //Physical Layer bit length
	ReceivedPayload        [M]float64 //Collected received payload
	TotalTransmissionTime  [M]float64 //Total transmission time
	TotalTransmissionPower [M]float64 //Total transmission power
	InstantEE              [M]float64 //Instant energy efficiency with time

	DR           [M]int //Current data rate
	txPowerIndex [M]int //ADR每次运行都是从最大值开始计算，而不需要current transmission power，这样无非可能增加循环次数，却使得处理方便了

	Frequency [M]int     //Frequency
	AvgSNR    [M]float64 //The average SNR of a device at all gateways with a single transmission
	AvgRSSI   [M]float64 //The average RSSI of a device at all gateways with a single transmission
	Fport     [M]string  //Fport

	HumiditySensor    [M]float64                                                                             //Humidity
	TemperatureSensor [M]float64                                                                             //Temperature
	CO2Sensor         [M]int                                                                                 //CO2
	TVOCSensor        [M]int                                                                                 //TVOC
	HumidityArray     = [...]float64{14.1, 15.3, 15.2, 14.5, 14.6, 15.8, 15.7, 14.9, 15.3, 15.0, 14.7, 14.6} //Fake Humidity initial status
	TemperatureArray  = [...]float64{23.1, 24.3, 24.2, 23.5, 23.6, 24.8, 24.7, 23.9, 24.3, 24.0, 23.7, 23.6} //Fake Temperature initial status
	CO2Array          = [...]int{450, 455, 439, 444, 445, 440, 451, 450, 455, 439, 444, 445}                 //Fake CO2 initial status
	TVOCArray         = []int{55, 53, 55, 54, 60, 61, 58, 58, 55, 53, 55, 54}                                //Fake TVOC initial status

	algorithm     = false //选择ADR或设计的算法
	algorithmName string
	SOTA1         = false //whether to use EFLoRa work
	SOTA2         = false //whether to use DyLoRa work

	R *rand.Rand //seed
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
		HumiditySensor struct {
			Num2 float64 `json:"2"`
		} `json:"humiditySensor"`
		IlluminanceSensor struct {
			Num3 int `json:"3"`
			Num4 int `json:"4"`
		} `json:"illuminanceSensor"`
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

	//Apply the packet reception ratio
	if PRR[ED] != 0 && rand.Float64() > PRR[ED] {
		return
	}

	//Get topic and payload
	fmt.Printf("MSG: %s\n", msg.Payload())
	//fmt.Printf("TOPIC: %s\n", msg.Topic())

	//Prase Json payload
	up := UP{}
	if err := json.Unmarshal(msg.Payload(), &up); err != nil {
		fmt.Printf("Message could not be parsed (%s): %s\n", msg.Payload(), err)
	}

	//Set the received SNR at different GW  to a value
	if N == 1 {
		up.Rxinfo[0].Lorasnr = -15.0
	} else if N == 2 {
		up.Rxinfo[0].Lorasnr = -15.0
		up.Rxinfo[1].Lorasnr = -10.0
	} else if N == 3 {
		up.Rxinfo[0].Lorasnr = -15.0
		up.Rxinfo[1].Lorasnr = -10.0
		up.Rxinfo[2].Lorasnr = -8.0
	} else if N == 4 {
		up.Rxinfo[0].Lorasnr = -15.0
		up.Rxinfo[1].Lorasnr = -10.0
		up.Rxinfo[2].Lorasnr = -8.0
		up.Rxinfo[3].Lorasnr = -11.0
	} else if N == 5 {
		up.Rxinfo[0].Lorasnr = -15.0
		up.Rxinfo[1].Lorasnr = -10.0
		up.Rxinfo[2].Lorasnr = -8.0
		up.Rxinfo[3].Lorasnr = -11.0
		up.Rxinfo[4].Lorasnr = -7.0
	} else if N == 6 {
		up.Rxinfo[0].Lorasnr = -15.0
		up.Rxinfo[1].Lorasnr = -10.0
		up.Rxinfo[2].Lorasnr = -8.0
		up.Rxinfo[3].Lorasnr = -11.0
		up.Rxinfo[4].Lorasnr = -7.0
		up.Rxinfo[5].Lorasnr = -9.0
	}

	//Get uplink SNR history
	RealSNRGain[ED] += SNRGain[ED]
	SNRGain[ED] = 0

	totalSNR := 0.0
	totalRSSI := 0.0

	for i, u := range up.Rxinfo {
		R = rand.New(rand.NewSource(int64(2*i+1) * time.Now().UnixNano()))
		u.Lorasnr = u.Lorasnr + RealSNRGain[ED]            //Apply the offset from assigned tp manually because there is no way to actually change the SNR
		u.Lorasnr = u.Lorasnr + getRandomSNR(2, -1, 10, 0) //Add random offset
		uplinkSNRHistory[ED][i] = append(uplinkSNRHistory[ED][i], u.Lorasnr)

		totalSNR += u.Lorasnr
		totalRSSI += u.Rssi

	}

	AvgSNR[ED] = totalSNR / N
	AvgRSSI[ED] = totalRSSI / N

	// If N is larger than up.Rxinfo, then uplinkSNRHistory[ED][i] will be zero by default. So set GW SNR adaptively to avoid the algorithm failing to converge when frame unreachable
	for j := len(up.Rxinfo); j <= N-1; j++ {
		//uplinkSNRHistory[ED][j] = append(uplinkSNRHistory[ED][j], -30)
		uplinkSNRHistory[ED][j] = append(uplinkSNRHistory[ED][j], uplinkSNRHistory[ED][0][0])
	}

	//Base64 decode 'data' field and calculate length
	decodeBytes, err := base64.StdEncoding.DecodeString(reflect.ValueOf(up).FieldByName("Data").String())
	if err != nil {
		log.Fatalln(err)
	}
	Lpayload[ED] = 8 * (float64(len(string(decodeBytes))) + 13)
	ReceivedPayload[ED] += Lpayload[ED]

	//Get frequency
	Frequency[ED] = int(reflect.ValueOf(up.Txinfo).FieldByName("Frequency").Int())

	//Get the real data rate
	//DR[ED] = int(reflect.ValueOf(up.Txinfo).FieldByName("Dr").Int())

	//Get the fake data rate
	DR[ED] = int(drAssigned[ED])

	sfExisiting[ED] = 12 - float64(DR[ED])
	if sfAssigned[ED] != 0 && sfExisiting[ED] != sfAssigned[ED] {
		GrpcAllocation(int(drAssigned[ED]), int(tpAssigned[ED]), 1, ED) //Remedial measures
	}

	getTotalTransmissionTimeandPower(ED)

	// Get real object
	//HumiditySensor[ED] = reflect.ValueOf(up.Object.TemperatureSensor).FieldByName("Num1").Float()
	//TemperatureSensor[ED] = reflect.ValueOf(up.Object.HumiditySensor).FieldByName("Num2").Float()
	//CO2Sensor[ED] = int(reflect.ValueOf(up.Object.IlluminanceSensor).FieldByName("Num3").Int())
	//TVOCSensor[ED] = int(reflect.ValueOf(up.Object.IlluminanceSensor).FieldByName("Num4").Int())

	// Get fake object
	if len(HumidityArray) < M {
		fmt.Printf("Len of fake object array should be padded to 'M'!\n")
		os.Exit(1)
	}
	HumiditySensor[ED] = HumidityArray[ED]
	TemperatureSensor[ED] = TemperatureArray[ED]
	CO2Sensor[ED] = CO2Array[ED]
	TVOCSensor[ED] = TVOCArray[ED]

	// Apply random offset
	HumiditySensor[ED] += 0.1 * getRandomFloat(21, -10)
	TemperatureSensor[ED] += 0.1 * getRandomFloat(21, -10)
	CO2Sensor[ED] += getRandomInt(3, -1)
	TVOCSensor[ED] += getRandomInt(3, -1)

	//Get port
	Fport[ED] = strconv.Itoa(int(reflect.ValueOf(up).FieldByName("Fport").Int()))

	//Count received messages
	fcnt[ED] = int(reflect.ValueOf(up).FieldByName("Fcnt").Int())
	UplinkFcntHistorySlice[ED] = append(UplinkFcntHistorySlice[ED], fcnt[ED])
	num[ED]++

	// Trigger the algorithm
	if num[ED] >= HISTORYCOUNT {
		//Get ACK bit flag
		adr[ED] = reflect.ValueOf(up).FieldByName("Adr").Bool()
		if adr[ED] == false {
			if algorithm == true {
				if SOTA1 == true {
					EFLoRa(Lpayload[ED], ED)
					algorithmName = "EFLoRa"
				} else if SOTA2 == true {
					if N != 1 {
						fmt.Printf("DyLoRa can only utilise a single gateway! This program will be shut down!\n")
						os.Exit(1)
					}
					DyLoRa(Lpayload[ED], ED)
					algorithmName = "DyLoRa"
				} else {
					SimulatedAnnealing(Lpayload[ED], ED)
					algorithmName = "EELoRa"
				}
			} else {
				if N != 1 {
					fmt.Printf("ADR can only utilise a single gateway! This program will be shut down!\n")
					os.Exit(1)
				}
				ADR(Lpayload[ED], DR[ED], txPowerIndex[ED], ED)
				algorithmName = "ADR"
			}
		} else {
			fmt.Printf("WARNING: ADR is disabled! This program will be shutdown!\n\n")
			os.Exit(1)
		}

		//Reset
		ResetSNRArray(ED)

		// Get statistics
		getPER(ED)
		printStatistic()
		Debuginfo(ED)
		logData(ED)

		// LinkADRReq
		GrpcAllocation(int(drAssigned[ED]), int(tpAssigned[ED]), 1, ED)

		//Get time
		AlgorithmSnaptime = time.Now()
		getAlgorithmRuntime()

		//Write to influxdb
		influxdbWriteAlgorithm(ED, SnapshotTime)

	}

	fmt.Printf("The number of received message: %d\n", num)
	fmt.Printf("Uplink SNR history: %v\n\n", uplinkSNRHistory)

	//Write to influxdb
	influxdbWrite(ED, SnapshotTime)
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
	//dBm2milliWatt(&TxpowerArrayWatt)
	fmt.Printf("TxpowerArrayWatt: %v\n", TxpowerArrayWatt)

	if algorithm == true {
		if SOTA1 == true {
			fmt.Printf("EFLoRa On!\n")
		} else if SOTA2 == true {
			fmt.Printf("DyLoRa On!\n")
		} else {
			fmt.Printf("EEADR On!\n")
		}
	} else {
		fmt.Printf("ADR On!\n")
	}

	//create ClientOptions struct setting the broker address, clientid, turn
	//off trace output and set the default message handler
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
