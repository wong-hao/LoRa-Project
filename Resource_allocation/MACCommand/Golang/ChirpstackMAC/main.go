package main

import (
	"ChirpstackMAC/src"
)


func main() {
	
	//TODO: 看network-server的configuration里的disable_mac_commands=true会禁止ADR与GRPC还是其中的某一个，以及和disable_adr=true的关系
	//TODO: 看confirmed message的确认是否会停止ADR设置的重传
	//TODO: 看未设置ADR时confirmed message未收到ACK会不会导致重传 (实验中似乎出现过)
	src.GRPC_Allocation(2,2,1)

}