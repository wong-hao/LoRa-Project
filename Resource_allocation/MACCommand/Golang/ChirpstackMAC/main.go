//https://www.chirpstack.io/application-server/api/go-examples/
//https://forum.chirpstack.io/t/issuing-linkadrreq-through-api/3727/2
//https://forum.chirpstack.io/t/method-to-send-downlink-mac-commands-to-end-node/3851
//https://www.bilibili.com/video/BV1ee411p7zu?t=111
//https://forum.chirpstack.io/t/the-linkadrreq-payload-enables-unexpected-uplink-channels/363/3

package main

import (
	"ChirpstackMAC/src"
)


func main() {

	src.GRPC_Allocation(2,3,1)

}