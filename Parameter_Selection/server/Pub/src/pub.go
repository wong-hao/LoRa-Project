//https://www.chirpstack.io/application-server/api/go-examples/
//https://forum.chirpstack.io/t/issuing-linkadrreq-through-api/3727/2
//https://forum.chirpstack.io/t/method-to-send-downlink-mac-commands-to-end-node/3851
//https://www.bilibili.com/video/BV1ee411p7zu?t=111
//https://forum.chirpstack.io/t/the-linkadrreq-payload-enables-unexpected-uplink-channels/363/3
//https://forum.chirpstack.io/t/adr-plugin-creation-example/11310/5

package src

import (
	"context"
	"fmt"
	"github.com/brocaar/chirpstack-api/go/v3/ns" //https://github.com/brocaar/chirpstack-api里面protobuf文件夹存放.proto原型文件仅供参考，实际调用go文件夹中编译好的.pb.go文件; 这与Quick start Python中的https://github.com/grpc/grpc/tree/master/examples
	"github.com/brocaar/lorawan"
	"google.golang.org/grpc"
	"math"
)

// configuration
const (
	// This must point to the API interface
	server = "106.14.134.224:8000"
)

var (
	// The DevEUI for which we want to enqueue the downlink
	devEUI1 = lorawan.EUI64{0x3b, 0xc1, 0xef, 0xb6, 0xe7, 0x19, 0xcc, 0x2c}
	devEUI2 = lorawan.EUI64{0x4a, 0x65, 0x99, 0x67, 0xaa, 0xfe, 0xe4, 0x71}
	devEUI3 = lorawan.EUI64{0xdc, 0xb3, 0x5c, 0xae, 0x79, 0x81, 0x48, 0xc0}
	devEUI4 = lorawan.EUI64{0x93, 0xfb, 0x28, 0x67, 0xfe, 0x31, 0xbd, 0x72}
	devEUI5 = lorawan.EUI64{0x1c, 0x3f, 0x1a, 0x2a, 0x40, 0xc6, 0xcd, 0x93}
	devEUI6 = lorawan.EUI64{0xa5, 0x06, 0x89, 0x34, 0x81, 0x64, 0x5d, 0xd3}

	devEUI = [...]lorawan.EUI64{devEUI1, devEUI2, devEUI3, devEUI4, devEUI5, devEUI6}

	TxpowerArrayWatt = []float64{79.43282347242814, 50.118723362727216, 31.622776601683793, 19.952623149688797, 12.589254117941676, 7.943282347242814, 5.011872336272725, 3.162277660168379}
)

// TODO: 看network-server的configuration里的disable_mac_commands=true是否会禁止ADR
// disable_adr=true或者disable_mac_commands=true后仍可以通过grpc发送MAC command
func GrpcAllocation(datarate int, txpower int, Nbtrans int, ED int) {
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
			DataRate: uint8(datarate),
			TXPower:  uint8(txpower),
			ChMask:   [16]bool{true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true},
			Redundancy: lorawan.Redundancy{
				ChMaskCntl: uint8(0),       //Channels 0 to 15
				NbRep:      uint8(Nbtrans), //Redundancy（与confirmed message规定的retransmission不同），fcnt相同，不会干扰到packet loss ratio
			},
		},
	}

	b, err := mac.MarshalBinary()
	//fmt.Printf("The b: %d\n", b)
	//fmt.Println(b)

	// make an MACCommand api call
	// no response: https://cloud.google.com/endpoints/docs/grpc/grpc-service-config
	resp, err := serviceClient.CreateMACCommandQueueItem(context.Background(), &ns.CreateMACCommandQueueItemRequest{
		DevEui:   devEUI[ED][:],
		Cid:      uint32(lorawan.LinkADRReq),
		Commands: [][]byte{b},
	})

	//https://stackoverflow.com/questions/67263020/how-to-call-a-grpc-function-which-return-empty/67263342#67263342
	_ = resp

	if err != nil {
		panic(err)
	}

	fmt.Printf("The MACCommand has been enqueued\n")

	sf := 9.0
	BW := 125000.0
	PRR := 0.980511
	RateCode := 0.8
	tpindex := 0

	compound1 := sf * BW * PRR * RateCode
	compound2 := math.Pow(2, sf) * TxpowerArrayWatt[tpindex]
	ee := compound1 / compound2
	fmt.Printf("The ee: %f\n", ee)
}
