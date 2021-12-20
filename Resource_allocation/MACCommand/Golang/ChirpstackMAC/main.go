package main

import (
	"ChirpstackMAC/src"
)

func main() {

	//TODO: 看network-server的configuration里的disable_mac_commands=true是否会禁止ADR
	//disable_adr=true或者disable_mac_commands=true后仍可以通过grpc发送MAC command
	//TODO: 看confirmed message的确认是否会停止ADR设置的重传
	//TODO: 未设置ADR时confirmed message未收到ACK不会导致重传，属于ARQ
	src.GrpcAllocation(3, 4, 1)

}
