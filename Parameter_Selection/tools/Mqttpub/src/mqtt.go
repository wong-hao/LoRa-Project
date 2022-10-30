package src

import (
	"fmt"
	"math/rand"
	"time"

	//import the Paho Go MQTT library
	MQTT "github.com/eclipse/paho.mqtt.golang"
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
	N            = 2  //Num of GW
	M            = 3  //Num of ED
	Tinterval    = 10 //Transmission interval

	MAXRoundtime = 2 // How much round run the algorithm
)

var (
	devtop1 = "application/1/device/3bc1efb6e719cc2c/event/up"
	devtop2 = "application/1/device/4a659967aafee471/event/up"
	devtop3 = "application/1/device/dcb35cae798148c0/event/up"
	devtop4 = "application/1/device/93fb2867fe31bd72/event/up"
	devtop5 = "application/1/device/1c3f1a2a40c6cd93/event/up"
	devtop6 = "application/1/device/a506893481645dd3/event/up"
	TOPIC   = [...]string{devtop1, devtop2, devtop3, devtop4, devtop5, devtop6}

	CLIENTID []string

	text string
)

// define a function for the default message handler
var f MQTT.MessageHandler = func(client MQTT.Client, msg MQTT.Message) {
	fmt.Printf("TOPIC: %s\n", msg.Topic())
	fmt.Printf("MSG: %s\n", msg.Payload())
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
	CLIENTID = append(CLIENTID, "mqtt-pub-client1")
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

	pub(c)

}

func pub(client MQTT.Client) {

	for Round := 0; Round <= MAXRoundtime-1; Round++ {
		for i := 0; i < HISTORYCOUNT; i++ {
			for j := 0; j < M; j++ {

				rand.Seed(int64(2*i*j+1) * time.Now().UnixNano())
				//stringvar1 := strconv.FormatFloat(-0.0, 'E', -1, 64)
				//stringvar2 := strconv.FormatFloat(-0.0, 'E', -1, 64)
				//stringvar3 := strconv.FormatFloat(-0.0, 'E', -1, 64)
				//stringvar4 := strconv.FormatFloat(-0.0, 'E', -1, 64)

				if N == 1 {
					text = " {\"applicationID\":\"6\",\"applicationName\":\"DraginoABP\",\"deviceName\":\"DraginoShield\",\"deviceProfileName\":\"DraginoABP\",\"deviceProfileID\":\"242002f1-46df-4feb-83c6-ec4b9aea138f\",\"devEUI\":\"3bc1efb6e719cc2c\",\"rxInfo\":[{\"gatewayID\":\"0016c001ff10d3f6\",\"uplinkID\":\"5a155a7c-16d0-45d1-bf46-b7c54dddbd58\",\"name\":\"raspberrypi1\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-78,\"loRaSNR\":15,\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}}],\"txInfo\":{\"frequency\":486900000,\"dr\":2},\"adr\":false,\"fCnt\":3,\"fPort\":1,\"data\":\"AWcAAAJoAAMCAAAEAgAA\",\"object\":{\"analogInput\":{\"3\":0,\"4\":0},\"humiditySensor\":{\"2\":0},\"temperatureSensor\":{\"1\":0}}}\n"
					//text = " {\"applicationID\":\"6\",\"applicationName\":\"DraginoABP\",\"deviceName\":\"DraginoShield\",\"deviceProfileName\":\"DraginoABP\",\"deviceProfileID\":\"242002f1-46df-4feb-83c6-ec4b9aea138f\",\"devEUI\":\"3bc1efb6e719cc2c\",\"rxInfo\":[{\"gatewayID\":\"0016c001ff10d3f6\",\"uplinkID\":\"5a155a7c-16d0-45d1-bf46-b7c54dddbd58\",\"name\":\"raspberrypi1\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-78,\"loRaSNR\":" + stringvar1 + ",\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}}],\"txInfo\":{\"frequency\":486900000,\"dr\":2},\"adr\":false,\"fCnt\":3,\"fPort\":1,\"data\":\"AWcAAAJoAAMCAAAEAgAA\",\"object\":{\"analogInput\":{\"3\":0,\"4\":0},\"humiditySensor\":{\"2\":0},\"temperatureSensor\":{\"1\":0}}}\n"
				} else if N == 2 {
					text = " {\"applicationID\":\"6\",\"applicationName\":\"DraginoABP\",\"deviceName\":\"DraginoShield\",\"deviceProfileName\":\"DraginoABP\",\"deviceProfileID\":\"242002f1-46df-4feb-83c6-ec4b9aea138f\",\"devEUI\":\"3bc1efb6e719cc2c\",\"rxInfo\":[{\"gatewayID\":\"0016c001ff10d3f7\",\"uplinkID\":\"24395e0e-2f51-4a18-ad6c-e9a67a1c7f5c\",\"name\":\"raspberrypi2\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-67,\"loRaSNR\":15,\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}},{\"gatewayID\":\"0016c001ff10d446\",\"uplinkID\":\"b7dd0d5c1586-4837-aaaa-19a6ef7a5d7c\",\"name\":\"raspberrypi4\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-66,\"loRaSNR\":15,\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}}],\"txInfo\":{\"frequency\":486900000,\"dr\":2},\"adr\":false,\"fCnt\":3,\"fPort\":1,\"data\":\"AWcAAAJoAAMCAAAEAgAA\",\"object\":{\"analogInput\":{\"3\":0,\"4\":0},\"humiditySensor\":{\"2\":0},\"temperatureSensor\":{\"1\":0}}}\n"
					//text = " {\"applicationID\":\"6\",\"applicationName\":\"DraginoABP\",\"deviceName\":\"DraginoShield\",\"deviceProfileName\":\"DraginoABP\",\"deviceProfileID\":\"242002f1-46df-4feb-83c6-ec4b9aea138f\",\"devEUI\":\"3bc1efb6e719cc2c\",\"rxInfo\":[{\"gatewayID\":\"0016c001ff10d3f7\",\"uplinkID\":\"24395e0e-2f51-4a18-ad6c-e9a67a1c7f5c\",\"name\":\"raspberrypi2\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-67,\"loRaSNR\":" + stringvar1 + ",\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}},{\"gatewayID\":\"0016c001ff10d446\",\"uplinkID\":\"b7dd0d5c1586-4837-aaaa-19a6ef7a5d7c\",\"name\":\"raspberrypi4\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-66,\"loRaSNR\":" + stringvar2 + ",\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}}],\"txInfo\":{\"frequency\":486900000,\"dr\":2},\"adr\":false,\"fCnt\":3,\"fPort\":1,\"data\":\"AWcAAAJoAAMCAAAEAgAA\",\"object\":{\"analogInput\":{\"3\":0,\"4\":0},\"humiditySensor\":{\"2\":0},\"temperatureSensor\":{\"1\":0}}}\n"
				} else if N == 3 {
					text = " {\"applicationID\":\"6\",\"applicationName\":\"DraginoABP\",\"deviceName\":\"DraginoShield\",\"deviceProfileName\":\"DraginoABP\",\"deviceProfileID\":\"242002f1-46df-4feb-83c6-ec4b9aea138f\",\"devEUI\":\"3bc1efb6e719cc2c\",\"rxInfo\":[{\"gatewayID\":\"0016c001ff10d3f7\",\"uplinkID\":\"24395e0e-2f51-4a18-ad6c-e9a67a1c7f5c\",\"name\":\"raspberrypi2\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-67,\"loRaSNR\":15,\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}},{\"gatewayID\":\"0016c001ff10d446\",\"uplinkID\":\"b7dd0d5c1586-4837-aaaa-19a6ef7a5d7c\",\"name\":\"raspberrypi4\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-66,\"loRaSNR\":15,\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}},{\"gatewayID\":\"0016c001ff10d42d\",\"uplinkID\":\"86d17570-51d9-47fb-80a3-3cc673bfbd8d\",\"name\":\"raspberrypi3\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-69,\"loRaSNR\":15,\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}}],\"txInfo\":{\"frequency\":486900000,\"dr\":2},\"adr\":false,\"fCnt\":3,\"fPort\":1,\"data\":\"AWcAAAJoAAMCAAAEAgAA\",\"object\":{\"analogInput\":{\"3\":0,\"4\":0},\"humiditySensor\":{\"2\":0},\"temperatureSensor\":{\"1\":0}}}\n"
					//text = " {\"applicationID\":\"6\",\"applicationName\":\"DraginoABP\",\"deviceName\":\"DraginoShield\",\"deviceProfileName\":\"DraginoABP\",\"deviceProfileID\":\"242002f1-46df-4feb-83c6-ec4b9aea138f\",\"devEUI\":\"3bc1efb6e719cc2c\",\"rxInfo\":[{\"gatewayID\":\"0016c001ff10d3f7\",\"uplinkID\":\"24395e0e-2f51-4a18-ad6c-e9a67a1c7f5c\",\"name\":\"raspberrypi2\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-67,\"loRaSNR\":" + stringvar1 + ",\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}},{\"gatewayID\":\"0016c001ff10d446\",\"uplinkID\":\"b7dd0d5c1586-4837-aaaa-19a6ef7a5d7c\",\"name\":\"raspberrypi4\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-66,\"loRaSNR\":" + stringvar2 + ",\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}},{\"gatewayID\":\"0016c001ff10d42d\",\"uplinkID\":\"86d17570-51d9-47fb-80a3-3cc673bfbd8d\",\"name\":\"raspberrypi3\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-69,\"loRaSNR\":" + stringvar3 + ",\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}}],\"txInfo\":{\"frequency\":486900000,\"dr\":2},\"adr\":false,\"fCnt\":3,\"fPort\":1,\"data\":\"AWcAAAJoAAMCAAAEAgAA\",\"object\":{\"analogInput\":{\"3\":0,\"4\":0},\"humiditySensor\":{\"2\":0},\"temperatureSensor\":{\"1\":0}}}\n"
				} else if N == 4 {
					text = " {\"applicationID\":\"6\",\"applicationName\":\"DraginoABP\",\"deviceName\":\"DraginoShield\",\"deviceProfileName\":\"DraginoABP\",\"deviceProfileID\":\"242002f1-46df-4feb-83c6-ec4b9aea138f\",\"devEUI\":\"3bc1efb6e719cc2c\",\"rxInfo\":[{\"gatewayID\":\"0016c001ff10d3f7\",\"uplinkID\":\"24395e0e-2f51-4a18-ad6c-e9a67a1c7f5c\",\"name\":\"raspberrypi2\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-67,\"loRaSNR\":15,\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}},{\"gatewayID\":\"0016c001ff10d446\",\"uplinkID\":\"b7dd0d5c1586-4837-aaaa-19a6ef7a5d7c\",\"name\":\"raspberrypi4\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-66,\"loRaSNR\":15,\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}},{\"gatewayID\":\"0016c001ff10d42d\",\"uplinkID\":\"86d17570-51d9-47fb-80a3-3cc673bfbd8d\",\"name\":\"raspberrypi3\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-69,\"loRaSNR\":15,\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}},{\"gatewayID\":\"0016c001ff10d42d\",\"uplinkID\":\"86d17570-51d9-47fb-80a3-3cc673bfbd8d\",\"name\":\"raspberrypi3\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-69,\"loRaSNR\":15,\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}}],\"txInfo\":{\"frequency\":486900000,\"dr\":2},\"adr\":false,\"fCnt\":3,\"fPort\":1,\"data\":\"AWcAAAJoAAMCAAAEAgAA\",\"object\":{\"analogInput\":{\"3\":0,\"4\":0},\"humiditySensor\":{\"2\":0},\"temperatureSensor\":{\"1\":0}}}\n"
					//text = " {\"applicationID\":\"6\",\"applicationName\":\"DraginoABP\",\"deviceName\":\"DraginoShield\",\"deviceProfileName\":\"DraginoABP\",\"deviceProfileID\":\"242002f1-46df-4feb-83c6-ec4b9aea138f\",\"devEUI\":\"3bc1efb6e719cc2c\",\"rxInfo\":[{\"gatewayID\":\"0016c001ff10d3f7\",\"uplinkID\":\"24395e0e-2f51-4a18-ad6c-e9a67a1c7f5c\",\"name\":\"raspberrypi2\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-67,\"loRaSNR\":" + stringvar1 + ",\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}},{\"gatewayID\":\"0016c001ff10d446\",\"uplinkID\":\"b7dd0d5c1586-4837-aaaa-19a6ef7a5d7c\",\"name\":\"raspberrypi4\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-66,\"loRaSNR\":" + stringvar2 + ",\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}},{\"gatewayID\":\"0016c001ff10d42d\",\"uplinkID\":\"86d17570-51d9-47fb-80a3-3cc673bfbd8d\",\"name\":\"raspberrypi3\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-69,\"loRaSNR\":" + stringvar3 + ",\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}}, {\"gatewayID\":\"0016c001ff10d42d\",\"uplinkID\":\"86d17570-51d9-47fb-80a3-3cc673bfbd8d\",\"name\":\"raspberrypi3\",\"time\":\"2022-02-16T14:14:23Z\",\"rssi\":-69,\"loRaSNR\":" + stringvar4 + ",\"location\":{\"latitude\":31.93664,\"longitude\":118.74151,\"altitude\":100}}],\"txInfo\":{\"frequency\":486900000,\"dr\":2},\"adr\":false,\"fCnt\":3,\"fPort\":1,\"data\":\"AWcAAAJoAAMCAAAEAgAA\",\"object\":{\"analogInput\":{\"3\":0,\"4\":0},\"humiditySensor\":{\"2\":0},\"temperatureSensor\":{\"1\":0}}}\n"
				}
				client.Publish(TOPIC[j], 0, false, text)
				time.Sleep(time.Duration(2) * time.Second)
			}
			time.Sleep(time.Duration(2) * time.Second)
		}
	}

}
