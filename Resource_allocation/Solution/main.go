//https://www.eclipse.org/paho/index.php?page=clients/golang/index.php

package main

import (
	"context"
	"encoding/json"
	"fmt"
	"github.com/brocaar/chirpstack-api/go/v3/ns"
	"github.com/brocaar/lorawan"
	"google.golang.org/grpc"
	"os/signal"
	"reflect"

	"syscall"
	"time"

	//import the Paho Go MQTT library
	MQTT "github.com/eclipse/paho.mqtt.golang"
	"os"
)

const (
	//TOPIC         = "ttt"
	TOPIC         = "application/1/device/53232c5e6c936483/event/#"

	QOS           = 0
	//SERVERADDRESS = "tcp://172.16.167.245:1883"
	SERVERADDRESS = "tcp://47.110.36.225:1883"

	CLIENTID      = "go_mqtt_client"

	WRITETOLOG  = true  // If true then received messages will be written to the console
	WRITETODISK = false // If true then received messages will be written to the file below

	OUTPUTFILE = "/binds/receivedMessages.txt"

	USERNAME	= "admin"
	PASSWORD	= "admin"

	HISTORYCOUNT = 6

	// This must point to the API interface
	server = "47.110.36.225:8000"
)

var (
	num  = 0
	messageJson [HISTORYCOUNT] string
	uplinkRssiHistory [HISTORYCOUNT] int
	uplinkDrHistory [HISTORYCOUNT] int
	dataArray [HISTORYCOUNT] string

	// The DevEUI for which we want to enqueue the downlink
	devEUI = lorawan.EUI64{0x53, 0x23, 0x2c, 0x5e, 0x6c, 0x93, 0x64, 0x83}
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
		Rssi      int       `json:"rssi"`
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
			uplinkRssiHistory[num] = u.Rssi
		}

		uplinkDrHistory[num] = int(reflect.ValueOf(up.Txinfo).FieldByName("Dr").Int())

		gRPC_Allocation(num+2,num+2)

	}else{
		for i := 0; i <= HISTORYCOUNT-2; i++ {
			messageJson[i] = messageJson[i+1]
			dataArray[i] = dataArray[i+1]
			uplinkRssiHistory[i] = uplinkRssiHistory[i+1]
			uplinkDrHistory[i] = uplinkDrHistory[i+1]
		}

		messageJson[HISTORYCOUNT-1] = string(msg.Payload())

		dataArray[HISTORYCOUNT-1] = reflect.ValueOf(up).FieldByName("Data").String()

		for _, u := range up.Rxinfo {
			uplinkRssiHistory[HISTORYCOUNT-1] = u.Rssi
		}

		uplinkDrHistory[HISTORYCOUNT-1] = int(reflect.ValueOf(up.Txinfo).FieldByName("Dr").Int())

	}
	num++

	//fmt.Printf("The number of received message: %d\n",num)
	//fmt.Printf("Received mssage: %v\n" , messageJson)
	fmt.Printf("Uplink Data history: %v\n" , dataArray)
	fmt.Printf("Uplink Rssi history: %v\n" , uplinkRssiHistory)
	fmt.Printf("Uplink Dr history: %v\n" , uplinkDrHistory)
}

var connectHandler MQTT.OnConnectHandler = func(client MQTT.Client) {
	fmt.Println("Connected")
}

var connectLostHandler MQTT.ConnectionLostHandler = func(client MQTT.Client, err error) {
	fmt.Printf("Connect lost: %v", err)
	client.Disconnect(250)

}

func main() {
	//create a ClientOptions struct setting the broker address, clientid, turn
	//off trace output and set the default message handler
	opts := MQTT.NewClientOptions().AddBroker(SERVERADDRESS).SetUsername(USERNAME).SetPassword(PASSWORD)
	opts.SetClientID(CLIENTID)
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

	sub(c);

	exit(c);

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

func gRPC_Allocation(datarate int, txpower int)  {
	// define gRPC dial options
	dialOpts := []grpc.DialOption{
		grpc.WithBlock(),
		grpc.WithInsecure(), // remove this when using TLS
	}

	// connect to the gRPC server
	conn, err := grpc.Dial(server, dialOpts...)
	if err != nil {
		panic(err)
	}

	// define the DeviceQueueService client
	serviceClient := ns.NewNetworkServerServiceClient(conn)

	mac := lorawan.MACCommand{
		CID: lorawan.LinkADRReq,
		Payload: &lorawan.LinkADRReqPayload{
			DataRate:	uint8(datarate), //TODO: ADR Plugin通过函数HandleResponse获得的metadata TXPower
			TXPower:	uint8(txpower), //TODO: 如果不能通过integration获得txpower看看能不能通过Network Controller，实在不行就设定一个全局变量存储初始值，然后加一减一
			//ChMask:		[16]bool{true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true},
			//Redundancy:	lorawan.Redundancy{
			//ChMaskCntl:	uint8(0),
			//NbRep:		uint8(5),
			//}, //TODO: Golang空出来的输入默认值是多少（根据Live frame好像全是0）；对比MACexample以及其他人写的MAC看是否能够空出来那哪些值
			//TODO: 看LoRaWAN Specification 1.0.2 中对Chmask、Redundancy的具体描述
			//TODO: 看ADR Plugin如何写Go语言的ADR程序，以及最后迫不得已直接上Plugin不用MAC Command了

		},
	}

	b,err :=mac.MarshalBinary()
	//fmt.Printf("The b: %d\n", b)
	fmt.Println(b)
	// make an MACCommand api call
	// no response: https://cloud.google.com/endpoints/docs/grpc/grpc-service-config
	resp, err := serviceClient.CreateMACCommandQueueItem(context.Background(), &ns.CreateMACCommandQueueItemRequest{
		DevEui:		devEUI[:],
		Cid:		uint32(lorawan.LinkADRReq),
		Commands:	[][]byte{b},	//TODO:看b数值具体是多少，用于python版的gRPC

	})

	//https://stackoverflow.com/questions/67263020/how-to-call-a-grpc-function-which-return-empty/67263342#67263342
	_ = resp

	if err != nil {
		panic(err)
	}

	fmt.Printf("The MACCommand has been enqueued")
}