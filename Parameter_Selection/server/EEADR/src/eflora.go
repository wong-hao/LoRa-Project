package src

import (
	"fmt"
	"time"
)

func EFLoRa(Lpayload float64, ED int) {

	AlgorithmInitTime = time.Now()

	fmt.Printf("Lpayload: %f\n", Lpayload)

	//Get SNR considering TP gain
	var AverageSNR [M][N]float64
	tpExisting[ED] = tpAssigned[ED]
	getAverageSNR(&AverageSNR)

	//Combination algorithm

	//Only try to increase the data rate to guarantee the fine-grained operations

	loopcount = 0

loop:

	// Starts from 11 in case of serious collision
	for sf := 11.0; sf >= 7.0; sf-- {

		//Only to reduce the transmission power
		for tpindex := 0; tpindex <= len(TxpowerArray)-1; tpindex++ {

			loopcount++

			getMsf(sf)
			EEb = getEE(Lpayload, sf, tpindex, TxpowerArrayWatt[tpindex], AverageSNR, ED, Msf)

			//Get last minEE
			getMinEE()
			lastexecutionminEE = minEE

			//Update EE and minEE if possible only when local EE is increased
			if EEb > EE[ED] {

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

				//fmt.Printf("minEE-lastminEE: %f\n", minEE-lastminEE)
				//Convergence condition based on threshold
				if minEE-lastexecutionminEE <= threshold {
					break loop //no need to send Mac command even all 48 combinations are traversed
				}
			}
		}
	}
	//Only DyLoRa will reach the end of code normally
	//SNRGain[ED] = 0.0
}
