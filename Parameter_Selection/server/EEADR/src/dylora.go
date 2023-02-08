package src

import (
	"fmt"
	"time"
)

func DyLoRa(Lpayload float64, ED int) {

	AlgorithmInitTime = time.Now()

	fmt.Printf("Lpayload: %f\n", Lpayload)

	//Get SNR considering TP gain
	var AverageSNR [M][N]float64
	tpExisting[ED] = tpAssigned[ED]
	getAverageSNR(&AverageSNR)

	//初始解
	sf := 12.0
	tpindex := 0

	maxEE := 0.0

	//Only try to increase the data rate to guarantee the fine-grained operations

	loopcount = 0

	for sfb := 12.0; sfb >= 7.0; sfb-- {

		//Only to reduce the transmission power
		for tpindexb := 0; tpindexb <= len(TxpowerArray)-1; tpindexb++ {

			loopcount++

			// Do not take collision into consideration
			EEb = getEE(Lpayload, sfb, tpindexb, TxpowerArrayWatt[tpindexb], AverageSNR, ED, 0)

			//Update EE and minEE if possible only when local EE is increased
			if EEb >= maxEE {
				maxEE = EEb

				sf = sfb
				tpindex = tpindexb
			}
		}
	}

	// Get the real collied result
	getMsf(sf)

	EE[ED] = getEE(Lpayload, sf, tpindex, TxpowerArrayWatt[tpindex], AverageSNR, ED, Msf)

	getRealM()

	getMinEE()

	sfAssigned[ED] = sf
	tpAssigned[ED] = float64(tpindex)

	drAssigned[ED] = 12 - sfAssigned[ED]

	getPER(ED)

	printStatistic()
	Debuginfo(ED)
	logData(ED)

	GrpcAllocation(int(drAssigned[ED]), int(tpAssigned[ED]), 1, ED)

	num[ED] = 0
	for i := 0; i < N; i++ {
		uplinkSNRHistory[ED][i] = uplinkSNRHistory[ED][i][0:0]
	}

	AlgorithmSnaptime = time.Now()
	getAlgorithmRuntime()

}
