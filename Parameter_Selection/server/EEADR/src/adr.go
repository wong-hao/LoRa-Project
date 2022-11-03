package src

import (
	"fmt"
	"math"
)

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
func ADR(Lpayload float64, dr int, txPowerIndex int, ED int) {
	fmt.Printf("Lpayload: %f\n", Lpayload)

	var AverageSNR [M][N]float64

	getAverageSNR(&AverageSNR)

	for i, j := range RequiredSNRForDRArray {
		if dr == i {
			RequiredSNRForDR = j
		}
	}

	//fmt.Printf("MaxSNR: %f\n", MaxSNR)

	snrMargin = getAllMaxSNR() - RequiredSNRForDR - margin_db

	nStep = int(math.Floor(snrMargin / 3))
	fmt.Printf("nStep: %d\n", nStep)

	if nStep < 0 { //the algorithm does not try to lower the data rate
		if txPowerIndex+nStep < 0 {
			loopcount = math.Abs(float64(nStep))
		} else {
			loopcount = float64(txPowerIndex)
		}
	} else {
		loopcount = float64(nStep) //The worst case when Txpower is in legal range and no early exit
	}

	getIdealTxPowerIndexAndDR(txPowerIndex, dr)
	sfAssigned[ED] = 12 - drAssigned[ED]

	getMsf(sfAssigned[ED])

	EE[ED] = getEE(Lpayload, sfAssigned[ED], txPowerIndex, TxpowerArrayWatt[int(tpAssigned[ED])], AverageSNR, ED, Msf)
	getRealM()

	//Get current minEE
	getMinEE()

	printStatistic()
	Debuginfo(ED)
	logData(ED)
	loopcount = 0

	GrpcAllocation(int(drAssigned[ED]), int(tpAssigned[ED]), 1, ED)

	num[ED] = 0
	for i := 0; i < N; i++ {
		uplinkSNRHistory[ED][i] = uplinkSNRHistory[ED][i][0:0]
	}
}

// Get max snr of single gateway
func getMaxSNR(slice []float64) float64 {
	snrM := -99999.0
	for _, m := range slice {
		if m > snrM {
			snrM = m
		}
	}
	return snrM
}

// Get max snr of all gateways
func getAllMaxSNR() float64 {
	snrM := -99999.0

	for k := 0; k < N; k++ {
		if getMaxSNR(uplinkSNRHistory[ED][k]) > snrM {
			snrM = getMaxSNR(uplinkSNRHistory[ED][k])
		}
	}

	return snrM
}

func getIdealTxPowerIndexAndDR(txPowerIndex int, dr int) {

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

	drAssigned[ED] = float64(dr)
	tpAssigned[ED] = float64(txPowerIndex)

}
