package main

import (
	"ChirpstackMAC/src"
)


func main() {
	
	//TODO: 看network-server的configuration里的disable_mac_commands=true会禁止ADR与GRPC还是其中的某一个，以及和disable_adr=true的关系
	src.GRPC_Allocation(2,2,3)

}