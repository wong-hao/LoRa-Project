package src

import (
	influxdb2 "github.com/influxdata/influxdb-client-go/v2"
	"github.com/influxdata/influxdb-client-go/v2/api/write"
	"strconv"
	"time"
)

const (
	InfluxdbPort = "8086"
	serverURL    = "http://" + SERVERADDRESS + ":" + InfluxdbPort //Aliyun

	authToken = "my-super-secret-auth-token"

	ApplicationName    = "DraginoABP"
	ReLoRaWANMethod    = "error_control"
	EEADRMethod        = "resource_allocation"
	TemperatureChannel = "1" // CayenneLPP channel
	HumidityChannel    = "2"
	GPSChannel         = "3"
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

// Chirpstack integration
// https://pkg.go.dev/github.com/influxdata/influxdb-client-go#readme-non-blocking-write-client
func influxdbWrite(ED int, SnapshotTime time.Time) {
	// Create client and set batch size to 20
	client := influxdb2.NewClientWithOptions(serverURL, authToken,
		influxdb2.DefaultOptions().SetBatchSize(20))
	// Get non-blocking write client
	writeAPI := client.WriteAPI("my-org", "my-bucket")

	// create points
	p1 := influxdb2.NewPoint(
		"device_uplink",
		map[string]string{
			"application_name": ApplicationName,
			"dev_eui":          deveui[ED],
			"device_name":      devname[ED],
			"dr":               strconv.Itoa(DR[ED]),
			"frequency":        strconv.Itoa(Frequency[ED]),
		},
		map[string]interface{}{
			"value": 1,
			"snr":   AvgSNR[ED],
			"rssi":  AvgRSSI[ED],
			"f_cnt": fcnt[ED],
		},
		SnapshotTime)

	p2 := influxdb2.NewPoint(
		"device_frmpayload_data_temperatureSensor_"+TemperatureChannel,
		map[string]string{
			"application_name": ApplicationName,
			"dev_eui":          deveui[ED],
			"device_name":      devname[ED],
			"f_port":           Fport[ED],
		},
		map[string]interface{}{
			"value": TemperatureSensor[ED],
		},
		SnapshotTime)

	p3 := influxdb2.NewPoint(
		"device_frmpayload_data_humiditySensor_"+HumidityChannel,
		map[string]string{
			"application_name": ApplicationName,
			"dev_eui":          deveui[ED],
			"device_name":      devname[ED],
			"f_port":           Fport[ED],
		},
		map[string]interface{}{
			"value": HumiditySensor[ED],
		},
		SnapshotTime)

	p4 := influxdb2.NewPoint(
		"device_frmpayload_data_gpsLocation_"+GPSChannel+"_location",
		map[string]string{
			"application_name": ApplicationName,
			"dev_eui":          deveui[ED],
			"device_name":      devname[ED],
			"f_port":           Fport[ED],
		},
		map[string]interface{}{
			"latitude":  Latitude[ED],
			"longitude": Longitude[ED],
			"geohash":   "none",
		},
		SnapshotTime)

	p5 := influxdb2.NewPoint(
		"device_frmpayload_data_gpsLocation_"+GPSChannel+"_altitude",
		map[string]string{
			"application_name": ApplicationName,
			"dev_eui":          deveui[ED],
			"device_name":      devname[ED],
			"f_port":           Fport[ED],
		},
		map[string]interface{}{
			"value": 0,
		},
		SnapshotTime)

	p := [...]*write.Point{p1, p2, p3, p4, p5}

	// write asynchronously
	for i := 0; i <= len(p)-1; i++ {
		writeAPI.WritePoint(p[i])
	}

	// Force all unwritten data to be sent
	writeAPI.Flush()
	// Ensures background processes finishes
	client.Close()
}

// Algorithm-based integration
func influxdbWriteAlgorithm(ED int, SnapshotTime time.Time) {
	// Create client and set batch size to 20
	client := influxdb2.NewClientWithOptions(serverURL, authToken,
		influxdb2.DefaultOptions().SetBatchSize(20))
	// Get non-blocking write client
	writeAPI := client.WriteAPI("my-org", "my-bucket")

	// create points

	//New algorithm-based integration (nothing to do with different end device)
	p1 := influxdb2.NewPoint(
		"device_frmpayload_data_statistics_"+EEADRChannel,
		map[string]string{
			"application_name": ApplicationName,
			"type":             EEADRMethod,
			"algorithm":        algorithmName,
		},
		map[string]interface{}{
			"minimal_energy_efficiency": minEE,
			"fairness_index":            Fairness,
		},
		SnapshotTime)

	// New algorithm-based integration (different with every end node)
	p2 := influxdb2.NewPoint(
		"device_frmpayload_data_statistics_"+EEADRChannel,
		map[string]string{
			"application_name": ApplicationName,
			"dev_eui":          deveui[ED],
			"device_name":      devname[ED],
			"type":             EEADRMethod,
			"algorithm":        algorithmName,
		},
		map[string]interface{}{
			"instant_energy_efficienty": EE[ED],
			"spreading_factor":          sfAssigned[ED],
			"transmission _power":       tpAssigned[ED],
			"packet_reception_ratio":    PRR[ED],
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
