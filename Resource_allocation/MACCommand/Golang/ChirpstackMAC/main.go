package main

import (
	"ChirpstackMAC/src"
)

func main() {

	//TODO: 看network-server的configuration里的disable_mac_commands=true是否会禁止ADR
	//disable_adr=true或者disable_mac_commands=true后仍可以通过grpc发送MAC command
	src.GrpcAllocation(3, 4, 1)

}
