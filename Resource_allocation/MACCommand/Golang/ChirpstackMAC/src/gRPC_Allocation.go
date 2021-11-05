//https://www.chirpstack.io/application-server/api/go-examples/
//https://forum.chirpstack.io/t/issuing-linkadrreq-through-api/3727/2
//https://forum.chirpstack.io/t/method-to-send-downlink-mac-commands-to-end-node/3851
//https://www.bilibili.com/video/BV1ee411p7zu?t=111
//https://forum.chirpstack.io/t/the-linkadrreq-payload-enables-unexpected-uplink-channels/363/3

package src

import (
	"context"
	"fmt"
	"github.com/brocaar/chirpstack-api/go/v3/ns" //https://github.com/brocaar/chirpstack-api里面protobuf文件夹存放.proto原型文件仅供参考，实际调用go文件夹中编译好的.pb.go文件; 这与Quick start Python中的https://github.com/grpc/grpc/tree/master/examples
	"github.com/brocaar/lorawan"
	"google.golang.org/grpc"


)

// configuration
const(
	// This must point to the API interface
	server = "106.14.134.224:8000"
)

var (
	// The DevEUI for which we want to enqueue the downlink
	//devEUI = lorawan.EUI64{0x53, 0x23, 0x2c, 0x5e, 0x6c, 0x93, 0x64, 0x83} //Rak811ABP
	//devEUI = lorawan.EUI64{0xd9, 0x30, 0xad, 0xe2, 0x99, 0x58, 0x2a, 0xb5} //Rak811OTAA
	//devEUI = lorawan.EUI64{0xc0, 0xe4, 0xec, 0xf4, 0xcd, 0x39, 0x9d, 0x55} //Rak4200ABP
	//devEUI = lorawan.EUI64{0x3d, 0xe0, 0x6c, 0x3b, 0x2b, 0x86, 0x70, 0x2a} //Rak4200OTAA
	//devEUI = lorawan.EUI64{0x3b, 0xc1, 0xef, 0xb6, 0xe7, 0x19, 0xcc, 0x2c} //DraginoABP
	devEUI = lorawan.EUI64{0xd1, 0xdb, 0x29, 0x91, 0x91, 0x5b, 0x9e, 0x9e} //DraginoOTAA
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
		Payload: &lorawan.LinkADRReqPayload{ //TODO: 注意Service-profiles / Device-profiles里的对DataRate / TXPower的最大最小值设置
			DataRate:	uint8(datarate), //TODO: ADR Plugin通过函数HandleResponse获得的metadata TXPower
			TXPower:	uint8(txpower), //TODO: 如果不能通过integration获得txpower看看能不能通过Network Controller，实在不行就设定一个全局变量存储初始值，然后加一减一
			ChMask:		[16]bool{true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true},
			Redundancy:	lorawan.Redundancy{
			//ChMaskCntl:	uint8(5),
			NbRep:		uint8(Nbtrans), //实测可以使用：接收到LinkAdrReq MAC指令后每次都会以3s间隔连续发送NbRep次，称之为重传；而每次重传发生终止的条件写在LoRaWAN Specification中
										//且重传的数据包fcnt相同，不会干扰到packet loss rate
			}, //TODO: Golang空出来的输入默认值是多少（根据Live frame好像全是0）；对比MACexample以及其他人写的MAC看是否能够空出来那哪些值
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
