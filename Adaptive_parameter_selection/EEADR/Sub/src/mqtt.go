//https://www.eclipse.org/paho/index.php?page=clients/golang/index.php
//https://github.com/eclipse/paho.mqtt.golang/issues/507
//https://www.emqx.cn/blog/how-to-use-mqtt-in-golang

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
	N            = 4  //Num of ED
	M            = 2  //Num of GW
	Tinterval    = 20 //Transmission interval
)

var (
	TOPICRak811ABP   = "application/1/device/53232c5e6c936483/event/up" //Rak811ABP
	TOPICRak811OTAA  = "application/2/device/d930ade299582ab5/event/up" //Rak811OTAA
	TOPICRak4200ABP  = "application/5/device/c0e4ecf4cd399d55/event/up" //Rak4200ABP
	TOPICRak4200OTAA = "application/8/device/3de06c3b2b86702a/event/up" //Rak4200OTAA
	TOPICDraginoABP  = "application/6/device/3bc1efb6e719cc2c/event/up" //DraginoABP
	TOPICDraginoABP2 = "application/6/device/3bc1efb6e719cc2d/event/up" //DraginoABP
	TOPICDraginoOTAA = "application/7/device/8bec4cec640c7c2a/event/up" //DraginoOTAA

	TOPIC = [...]string{TOPICDraginoABP, TOPICDraginoABP2, TOPICDraginoOTAA, TOPICRak811ABP, TOPICRak811OTAA, TOPICRak4200ABP, TOPICRak4200OTAA}

	CLIENTID = [...]string{"0", "1"}

	num = [M]int{0, 0} //num of received message
	ED  int

	uplinkSNRHistory [M][N][]float64

	ADR_ACK_Req [M]bool

	Lpayload [M]float64 //bit
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
	up := UP{}
	if err := json.Unmarshal(msg.Payload(), &up); err != nil {
		fmt.Printf("Message could not be parsed (%s): %s", msg.Payload(), err)
	}

	decodeBytes, err := base64.StdEncoding.DecodeString(reflect.ValueOf(up).FieldByName("Data").String())
	if err != nil {
		log.Fatalln(err)
	}

	OptionsReader := client.OptionsReader()
	ClientID := OptionsReader.ClientID()
	ED, _ = strconv.Atoi(ClientID)

	Lpayload[ED] = 8 * (float64(len(string(decodeBytes))) + 13)

	for i, u := range up.Rxinfo {
		uplinkSNRHistory[ED][i] = append(uplinkSNRHistory[ED][i], u.Lorasnr)
	}

	num[ED]++

	if num[ED] == HISTORYCOUNT {
		ADR_ACK_Req[ED] = reflect.ValueOf(up).FieldByName("Adr").Bool()
		if ADR_ACK_Req[ED] == true {
			getCombination(Lpayload[ED], ED)
			num[ED] = 0 //every HISTORYCOUNT run once
			for i := 0; i < N; i++ {
				uplinkSNRHistory[ED][i] = uplinkSNRHistory[ED][i][0:0]
			}
		} else {
			fmt.Printf("WARNING: ACK is disabled!\n\n")
		}
	}

	//fmt.Printf("TOPIC: %s\n", msg.Topic())
	fmt.Printf("MSG: %s\n", msg.Payload())
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

	//create a ClientOptions struct setting the broker address, clientid, turn
	//off trace output and set the default message handler
	opts := MQTT.NewClientOptions().AddBroker(SERVERADDRESS).SetUsername(USERNAME).SetPassword(PASSWORD)
	opts.SetClientID(CLIENTID[0])
	opts.SetDefaultPublishHandler(f)
	opts.OnConnect = connectHandler
	opts.OnConnectionLost = connectLostHandler
	opts.ConnectRetry = true
	opts.AutoReconnect = true

	//create and start a client using the above ClientOptions
	c := MQTT.NewClient(opts)
	if token := c.Connect(); token.Wait() && token.Error() != nil {
		panic(token.Error())
	}

	opts2 := MQTT.NewClientOptions().AddBroker(SERVERADDRESS).SetUsername(USERNAME).SetPassword(PASSWORD)
	opts2.SetClientID(CLIENTID[1])
	opts2.SetDefaultPublishHandler(f)
	opts2.OnConnect = connectHandler
	opts2.OnConnectionLost = connectLostHandler
	opts2.ConnectRetry = true
	opts2.AutoReconnect = true

	c2 := MQTT.NewClient(opts2)
	if token := c2.Connect(); token.Wait() && token.Error() != nil {
		panic(token.Error())
	}

	sub(c)
	sub(c2)

	dB2Watt(TxpowerArray, &TxpowerArrayWatt)
	fmt.Printf("TxpowerArrayWatt: %v\n", TxpowerArrayWatt)

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
