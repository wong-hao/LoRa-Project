//https://www.eclipse.org/paho/index.php?page=clients/golang/index.php

package main

import (
	"fmt"
	"time"

	//import the Paho Go MQTT library
	MQTT "github.com/eclipse/paho.mqtt.golang"
	"os"
)

//define a function for the default message handler
var f MQTT.MessageHandler = func(client MQTT.Client, msg MQTT.Message) {
	fmt.Printf("TOPIC: %s\n", msg.Topic())
	fmt.Printf("MSG: %s\n", msg.Payload())
}

func main1() {
	//create a ClientOptions struct setting the broker address, clientid, turn
	//off trace output and set the default message handler
	opts := MQTT.NewClientOptions().AddBroker("tcp://47.110.36.225:1883").SetUsername("admin").SetPassword("admin")
	opts.SetClientID("643a68881fa6462c9f90a8d90fd46a1b")
	opts.SetDefaultPublishHandler(f)

	//create and start a client using the above ClientOptions
	c := MQTT.NewClient(opts)
	if token := c.Connect(); token.Wait() && token.Error() != nil {
		panic(token.Error())
	}

	//subscribe to the topic /go-mqtt/sample and request messages to be delivered
	//at a maximum qos of zero, wait for the receipt to confirm the subscription
	if token := c.Subscribe("application/3/device/53232c5e6c936483/event/#", 0, nil); token.Wait() && token.Error() != nil {
		fmt.Println(token.Error())
		os.Exit(1)
	}

	//Publish 5 messages to /go-mqtt/sample at qos 1 and wait for the receipt
	//from the server after sending each message
	//for i := 0; i < 5; i++ {
	//	text := fmt.Sprintf("this is msg #%d!", i)
	//	token := c.Publish("go-mqtt/sample", 0, false, text)
	//	token.Wait()
	//}

	time.Sleep(3 * time.Second)

	//unsubscribe from /go-mqtt/sample
	//if token := c.Unsubscribe("application/3/device/53232c5e6c936483/event/#"); token.Wait() && token.Error() != nil {
	//	fmt.Println(token.Error())
	//	os.Exit(1)
	//}

	//c.Disconnect(250)
}
