package main

import (
	"context"
	"fmt"

	"google.golang.org/grpc"

	"github.com/brocaar/chirpstack-api/go/v3/ns"
	"github.com/brocaar/lorawan"
)

// configuration
var (
	// This must point to the API interface
	server = "47.110.36.225:8000"

	// The DevEUI for which we want to enqueue the downlink
	devEUI = lorawan.EUI64{0x53, 0x23, 0x2c, 0x5e, 0x6c, 0x93, 0x64, 0x83}

	// The API token (retrieved using the web-interface)
	apiToken = "..."
)

type APIToken string

func (a APIToken) GetRequestMetadata(ctx context.Context, url ...string) (map[string]string, error) {
	return map[string]string{
		"authorization": fmt.Sprintf("Bearer %s", a),
	}, nil
}

func (a APIToken) RequireTransportSecurity() bool {
	return false
}

func main() {
	// define gRPC dial options
	dialOpts := []grpc.DialOption{
		grpc.WithBlock(),
		grpc.WithPerRPCCredentials(APIToken(apiToken)),
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
			DataRate: uint8(2),
			TXPower: uint8(2),
			ChMask: [16]bool{true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true},
			Redundancy: lorawan.Redundancy{
				ChMaskCntl: uint8(0),
				NbRep: uint8(2),
			},

		},
	}

	b,err :=mac.MarshalBinary()

	// make an Enqueue api call
	resp, err := serviceClient.CreateMACCommandQueueItem(context.Background(), &ns.CreateMACCommandQueueItemRequest{
		DevEui:    devEUI[:],
		Cid:     uint32(lorawan.LinkADRReq),
		Commands:  [][]byte{b},

	})
	if err != nil {
		panic(err)
	}

	fmt.Printf("The downlink has been enqueued with FCnt: %d\n", resp)
}