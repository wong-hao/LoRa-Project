//https://www.eclipse.org/paho/index.php?page=clients/golang/index.php
//https://github.com/eclipse/paho.mqtt.golang/issues/507
//https://www.emqx.cn/blog/how-to-use-mqtt-in-golang

package src

import (
	"encoding/json"
	"fmt"
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
	TOPIC         = "ttt"
	//TOPIC         = "application/1/device/53232c5e6c936483/event/#"

	QOS           = 0
	SERVERADDRESS = "tcp://172.16.165.207:1883"
	//SERVERADDRESS = "tcp://47.110.36.225:1883"

	CLIENTID      = "go_mqtt_client"
	CLIENTID2      = "go_mqtt_client2"

	WRITETOLOG  = true  // If true then received messages will be written to the console
	WRITETODISK = false // If true then received messages will be written to the file below

	OUTPUTFILE = "/binds/receivedMessages.txt"

	USERNAME	= "admin"
	PASSWORD	= "admin"

	HISTORYCOUNT = 6

)

var (
	num  = 0
	DR int
	Txpower  = maxTxPower

	messageJson [HISTORYCOUNT] string
	dataArray [HISTORYCOUNT] string
	uplinkSNRHistory [HISTORYCOUNT] float64
	uplinkFcntHistory [HISTORYCOUNT] int

	ADR_ACK_Req bool

	NbTrans int = 1


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
		Rssi      float64       `json:"rssi"`
		Lorasnr   float64   `json:"loRaSNR"`
		Location  struct {
			Latitude  int `json:"latitude"`
			Longitude int `json:"longitude"`
			Altitude  int `json:"altitude"`
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

	}else{
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
	fmt.Printf("Uplink Data history: %v\n" , dataArray)
	fmt.Printf("Uplink SNR history: %v\n" , uplinkSNRHistory)
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

	sub(c);
	sub(c2);

	exit(c);
	exit(c2);


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
	if token := client .Subscribe(TOPIC, QOS, nil); token.Wait() && token.Error() != nil {
		fmt.Println(token.Error())
		os.Exit(1)
	}

}

func exit(clinet MQTT.Client){

	// Messages will be delivered asynchronously so we just need to wait for a signal to shutdown
	sig := make(chan os.Signal, 1)
	signal.Notify(sig, os.Interrupt)
	signal.Notify(sig, syscall.SIGTERM)

	<-sig
	fmt.Println("signal caught - exiting")
	clinet.Disconnect(1000)
	fmt.Println("shutdown complete")

}
