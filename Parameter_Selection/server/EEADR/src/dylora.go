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

	// Get assigned parameters
	sfAssigned[ED] = sf
	tpAssigned[ED] = float64(tpindex)

	drAssigned[ED] = 12 - sfAssigned[ED]

	// Get the real collied result
	getMsf(sfAssigned[ED])

	EE[ED] = getEE(Lpayload, sfAssigned[ED], int(tpAssigned[ED]), TxpowerArrayWatt[int(tpAssigned[ED])], AverageSNR, ED, Msf)

	//Get current minEE
	getRealM()

	getMinEE()

}
