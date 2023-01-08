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

// https://pkg.go.dev/github.com/influxdata/influxdb-client-go#readme-non-blocking-write-client
func influxdbWrite(ED int, SnapshotTime time.Time) {
	// Create client and set batch size to 20
	client := influxdb2.NewClientWithOptions(serverURL, authToken,
		influxdb2.DefaultOptions().SetBatchSize(20))
	// Get non-blocking write client
	writeAPI := client.WriteAPI("my-org", "my-bucket")

	// create points
	p1 := influxdb2.NewPoint(
		"device_frmpayload_data_statistics",
		map[string]string{
			"application_name": "DraginoABP",
			"type":             "error_control",
		},
		map[string]interface{}{
			"packcet_delivery_ratio": PDR,
			"throughput":             Throughput,
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
