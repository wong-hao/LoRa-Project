//https://www.eclipse.org/paho/index.php?page=clients/golang/index.php
//https://www.emqx.com/zh/blog/how-to-use-mqtt-in-golang

package src

import (
	"encoding/base64"
	"encoding/json"
	"fmt"
	"log"
	"reflect"
	"strconv"

	//"github.com/shopspring/decimal"
	"os/signal"
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

	HISTORYCOUNT = 6  //Recent SNR history num
	N            = 4  //Num of GW
	M            = 2  //Num of ED
	Tinterval    = 20 //Transmission interval
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
	CLIENTID = [...]string{"0", "1"}

	opts = [M]*MQTT.ClientOptions{} //mqtt option array
	c    = [M]MQTT.Client{}         //mqtt client array

	num = [M]int{0, 0} //num of received message
	ED  int            //ED flag

	uplinkSNRHistory [M][N][]float64
	adr              [M]bool //ACK bit

	Lpayload [M]float64 //Bit length

	DR           [M]int         //Current data rate
	txPowerIndex = [M]int{0, 0} //ADR每次运行都是从最大值开始计算，而不需要current transmission power，这样无非可能增加循环次数，却使得处理方便了
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

	//Get topic and payload
	fmt.Printf("Content: %s\n", msg.Payload())
	//fmt.Printf("TOPIC: %s\n", msg.Topic())

	//Prase Json payload
	up := UP{}
	if err := json.Unmarshal(msg.Payload(), &up); err != nil {
		fmt.Printf("Message could not be parsed (%s): %s", msg.Payload(), err)
	}

	//Get uplink SNR history
	for i, u := range up.Rxinfo {
		uplinkSNRHistory[ED][i] = append(uplinkSNRHistory[ED][i], u.Lorasnr)
	}

	//Base64 decode 'data' field and calculate length
	decodeBytes, err := base64.StdEncoding.DecodeString(reflect.ValueOf(up).FieldByName("Data").String())
	if err != nil {
		log.Fatalln(err)
	}
	Lpayload[ED] = 8 * (float64(len(string(decodeBytes))) + 13)

	//Get current data rate
	DR[ED] = int(reflect.ValueOf(up.Txinfo).FieldByName("Dr").Int())

	//Count received messages
	num[ED]++

	if num[ED] == HISTORYCOUNT {
		//Get ACK bit flag
		adr[ED] = reflect.ValueOf(up).FieldByName("Adr").Bool()
		if adr[ED] == true {
			//ADR(Lpayload[ED], DR[ED], txPowerIndex[ED], ED)
			EEADR(Lpayload[ED], ED)
		} else {
			fmt.Printf("WARNING: ACK is disabled! This program will be shutdown!\n\n")
		}

		//Run every HISTORYCOUNT messages once
		num[ED] = 0
		for i := 0; i < N; i++ {
			uplinkSNRHistory[ED][i] = uplinkSNRHistory[ED][i][0:0]
		}
	}

	fmt.Printf("The number of received message: %d\n", num)
	fmt.Printf("Uplink SNR history: %v\n\n", uplinkSNRHistory)

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

	dBm2milliWatt(TxpowerArray, &TxpowerArrayWatt)
	fmt.Printf("TxpowerArrayWatt: %v\n", TxpowerArrayWatt)

	fmt.Printf("ED num: %d, GW num: %d\n", M, N)

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
