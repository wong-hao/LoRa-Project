package src

import (
	influxdb2 "github.com/influxdata/influxdb-client-go/v2"
	"github.com/influxdata/influxdb-client-go/v2/api/write"
	"time"
)

const (
	InfluxdbPort = "8086"
	serverURL    = "http://" + SERVERADDRESS + ":" + InfluxdbPort //Aliyun

	authToken = "my-super-secret-auth-token"

	ApplicationName    = "DraginoABP"
	TemperatureChannel = "1" // CayenneLPP channel
	HumidityChannel    = "2"
	CO2Channel         = "3"
	TVOCChannel        = "4"
	ReLoRaWANChannel   = "1"
	EEADRChannel       = "2"
)

var (
	// The DevEUI
	deveui1  = "3bc1efb6e719cc2c"
	deveui2  = "4a659967aafee471"
	deveui3  = "dcb35cae798148c0"
	deveui4  = "93fb2867fe31bd72"
	deveui5  = "1c3f1a2a40c6cd93"
	deveui6  = "a506893481645dd3"
	deveui7  = "4bc0e966236b8a35"
	deveui8  = "6f6bc87b6d5762c1"
	deveui9  = "c2674e40266c1555"
	deveui10 = "724ab434d984f3cc"
	deveui11 = "cb0a13fab5eb1d77"
	deveui12 = "8163ad2a34af4626"

	deveui = [...]string{deveui1, deveui2, deveui3, deveui4, deveui5, deveui6, deveui7, deveui8, deveui9, deveui10, deveui11, deveui12}

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
	devname11 = "device11"
	devname12 = "device12"

	devname = [...]string{devname1, devname2, devname3, devname4, devname5, devname6, devname7, devname8, devname9, devname10, devname11, devname12}
)

// Chirpstack integration
// https://pkg.go.dev/github.com/influxdata/influxdb-client-go#readme-non-blocking-write-client

// Optimization-based integration
func influxdbWriteOptimization(SnapshotTime time.Time) {
	// Create client and set batch size to 20
	client := influxdb2.NewClientWithOptions(serverURL, authToken,
		influxdb2.DefaultOptions().SetBatchSize(20))
	// Get non-blocking write client
	writeAPI := client.WriteAPI("my-org", "optimizations")

	// create points

	//New Optimization-based integration (nothing to do with different end device)
	p1 := influxdb2.NewPoint(
		"device_frmpayload_data_statistics_"+ReLoRaWANChannel,
		map[string]string{
			"application_name":  ApplicationName,
			"optimization_name": OptimizationName,
		},
		map[string]interface{}{
			"stat1":  stat1,
			"stat2":  stat2,
			"stat3":  stat3,
			"stat4":  stat4,
			"result": result,
		},
		SnapshotTime)

	p := [...]*write.Point{p1}

	// write asynchronously
	for i := 0; i <= len(p)-1; i++ {
		writeAPI.WritePoint(p[i])
	}

	// Force all unwritten data to be sent
	writeAPI.Flush()
	// Ensures background processes finishes
	client.Close()
}
