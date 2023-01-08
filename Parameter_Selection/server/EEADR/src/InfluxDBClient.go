package src

import (
	influxdb2 "github.com/influxdata/influxdb-client-go/v2"
	"github.com/influxdata/influxdb-client-go/v2/api/write"
	"time"
)

const (
	serverURL = "http://106.14.134.224:8086" //Aliyun
	authToken = "my-super-secret-auth-token"
)

var (
	// The DevEUI
	deveui1  = "3bc1efb6e719cc2c"
	deveui2  = "4a659967aafee471"
	deveui3  = "dcb35cae798148c0"
	deveui4  = "93fb2867fe31bd72"
	deveui5  = "1c3f1a2a40c6cd93"
	deveui6  = "a506893481645dd3"
	deveui7  = "a506893481645dd4"
	deveui8  = "a506893481645dd5"
	deveui9  = "a506893481645dd6"
	deveui10 = "a506893481645dd7"

	deveui = [...]string{deveui1, deveui2, deveui3, deveui4, deveui5, deveui6, deveui7, deveui8, deveui9, deveui10}

	devname1  = "device1"
	devname2  = "device2"
	devname3  = "device3"
	devname4  = "device4"
	devname5  = "device5"
	devname6  = "device6"
	devname7  = "device7"
	devname8  = "device8"
	devname9  = "device9"
	devname10 = "device10"

	devname = [...]string{devname1, devname2, devname3, devname4, devname5, devname6, devname7, devname8, devname9, devname10}
)

// https://pkg.go.dev/github.com/influxdata/influxdb-client-go#readme-non-blocking-write-client
func influxdbWrite(ED int, SnapshotTime time.Time) {
	// Create client and set batch size to 20
	client := influxdb2.NewClientWithOptions(serverURL, authToken,
		influxdb2.DefaultOptions().SetBatchSize(20))
	// Get non-blocking write client
	writeAPI := client.WriteAPI("my-org", "my-bucket")

	// create points
	//Nothing to do with different end device
	p1 := influxdb2.NewPoint(
		"device_frmpayload_data_statistics",
		map[string]string{
			"application_name": "DraginoABP",
			"type":             "resource_allocation",
			"algorithm":        algorithmName,
		},
		map[string]interface{}{
			"minimal_energy_efficiency": minEE,
			"fairness_index":            Fairness,
		},
		SnapshotTime)

	// Different with every end node
	p2 := influxdb2.NewPoint(
		"device_frmpayload_data_statistics",
		map[string]string{
			"application_name": "DraginoABP",
			"dev_eui":          deveui[ED],
			"device_name":      devname[ED],
			"type":             "resource_allocation",
			"algorithm":        algorithmName,
		},
		map[string]interface{}{
			"instant_energy_efficienty": EE[ED],
			"spreading_factor":          sfAssigned[ED],
			"transmission _power":       tpAssigned[ED],
		},
		SnapshotTime)

	p := [...]*write.Point{p1, p2}

	// write asynchronously
	for i := 0; i <= len(p)-1; i++ {
		writeAPI.WritePoint(p[i])
	}

	// Force all unwritten data to be sent
	writeAPI.Flush()
	// Ensures background processes finishes
	client.Close()
}
