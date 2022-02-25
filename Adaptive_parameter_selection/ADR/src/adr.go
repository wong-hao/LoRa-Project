package src

import "math"

const (
	margin_db       = 10
	maxDR           = 5
	maxTxPowerIndex = 0
	minTxPowerIndex = 7
)

var (
	RequiredSNRForDR      float64
	RequiredSNRForDRArray = [...]float64{-20, -17.5, -15, -12.5, -10, -7.5}

	snrMargin float64
	nStep     int
)

// ADR https://github.com/brocaar/chirpstack-network-server/blob/4e7fdb348b5d465c8faacbf6a1f6f5fabea88066/internal/adr/default.go#L18
func ADR(dr int, txPowerIndex int, ED int) {

	for i, j := range RequiredSNRForDRArray {
		if dr == i {
			RequiredSNRForDR = j
		}
	}

	snrM := -99999.0
	//Get max snr of all gateways
	for k := 0; k < N; k++ {
		if getMaxSNR(uplinkSNRHistory[ED][k]) > snrM {
			snrM = getMaxSNR(uplinkSNRHistory[ED][k])
		}
	}

	//fmt.Printf("MaxSNR: %f\n", MaxSNR)

	snrMargin = snrM - RequiredSNRForDR - margin_db

	nStep = int(math.Floor(snrMargin / 3))

	dr, txPowerIndex = getIdealTxPowerIndexAndDR(nStep, txPowerIndex, dr)

	//TODO: 看network-server的configuration里的disable_mac_commands=true是否会禁止ADR
	//disable_adr=true或者disable_mac_commands=true后仍可以通过grpc发送MAC command
	//GrpcAllocation(dr, txPowerIndex, 1, ED)
}

//Get max snr of single gateway
func getMaxSNR(slice []float64) float64 {
	snrM := -99999.0
	for _, m := range slice {
		if m > snrM {
			snrM = m
		}
	}
	return snrM
}

func getIdealTxPowerIndexAndDR(nStep int, txPowerIndex int, dr int) (int, int) {

	//while: https://www.jianshu.com/p/2ac52fe2810e
	for {
		if nStep == 0 {
			break
		} else if nStep > 0 {
			if dr < maxDR {
				dr++
			} else {
				txPowerIndex++
			}
			nStep--
			if txPowerIndex == minTxPowerIndex {
				break
			}
		} else if nStep < 0 {
			if txPowerIndex < maxTxPowerIndex {
				txPowerIndex--
			} else {
				break
			}
			nStep++
		}
	}

	return dr, txPowerIndex

}
