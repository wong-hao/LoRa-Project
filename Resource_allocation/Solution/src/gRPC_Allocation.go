package src

import (
	"context"
	"fmt"
	"github.com/brocaar/chirpstack-api/go/v3/ns"
	"github.com/brocaar/lorawan"
	"google.golang.org/grpc"


)

// configuration
const(
	// This must point to the API interface
	server = "47.110.36.225:8000"
)

var (
	// The DevEUI for which we want to enqueue the downlink
	devEUI = lorawan.EUI64{0x53, 0x23, 0x2c, 0x5e, 0x6c, 0x93, 0x64, 0x83}
)

func GRPC_Allocation(datarate int, txpower int, Nbtrans int)  {
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
			Redundancy:	lorawan.Redundancy{
			//ChMaskCntl:	uint8(0),
			NbRep:		uint8(Nbtrans), //实测可以使用：接收到LinkAdrReq MAC指令后每次都会以3s间隔连续发送NbRep次，称之为重传；而每次重传发生终止的条件写在LoRaWAN Specification中
			//且重传的数据包fcnt相同，不会干扰到packet loss rate			}, //TODO: Golang空出来的输入默认值是多少（根据Live frame好像全是0）；对比MACexample以及其他人写的MAC看是否能够空出来那哪些值
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

	fmt.Printf("The MACCommand has been enqueued\n")
}
