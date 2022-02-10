package src

import "fmt"

func single(sf int) {
	fmt.Printf("Average SNR: %f\n", getAverageSNR(uplinkSNRHistory))
}
