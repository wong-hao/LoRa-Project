//https://www.chirpstack.io/application-server/api/go-examples/
//https://forum.chirpstack.io/t/issuing-linkadrreq-through-api/3727/2
//https://www.bilibili.com/video/BV1ee411p7zu?t=111
//https://forum.chirpstack.io/t/the-linkadrreq-payload-enables-unexpected-uplink-channels/363/3

package main

import (
	"context"
	"fmt"

	"google.golang.org/grpc"

	/*	https://github.com/brocaar/chirpstack-api里面protobuf文件夹存放.proto原型文件仅供参考，实际调用go文件夹中编译好的.pb.go文件
		这与Quick start Python中的https://github.com/grpc/grpc/tree/master/examples
	*/
	"github.com/brocaar/chirpstack-api/go/v3/ns"
	"github.com/brocaar/lorawan"
)

// configuration
var (
	// This must point to the API interface
	server = "47.110.36.225:8000"

	// The DevEUI for which we want to enqueue the downlink
	devEUI = lorawan.EUI64{0x53, 0x23, 0x2c, 0x5e, 0x6c, 0x93, 0x64, 0x83}
)

func main() {
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
			DataRate:	uint8(3),
			TXPower:	uint8(3),
			//ChMask:		[16]bool{true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true},
			Redundancy:	lorawan.Redundancy{
				ChMaskCntl:	uint8(0),
				NbRep:		uint8(2),
			},

		},
	}

	b,err :=mac.MarshalBinary()

	// make an MACCommand api call
	// no response: https://cloud.google.com/endpoints/docs/grpc/grpc-service-config
	resp, err := serviceClient.CreateMACCommandQueueItem(context.Background(), &ns.CreateMACCommandQueueItemRequest{
		DevEui:		devEUI[:],
		Cid:		uint32(lorawan.LinkADRReq),
		Commands:	[][]byte{b},

	})

	//https://stackoverflow.com/questions/67263020/how-to-call-a-grpc-function-which-return-empty/67263342#67263342
	_ = resp

	if err != nil {
		panic(err)
	}

	fmt.Printf("The MACCommand has been enqueued")
}