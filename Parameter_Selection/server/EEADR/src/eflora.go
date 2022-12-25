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

	for sf := 11.0; sf >= 7.0; sf-- {

		getMsf(sf)

		//Only to reduce the transmission power
		for tpindex := 0; tpindex <= len(TxpowerArray)-1; tpindex++ {

			loopcount++

			//Get last minEE
			getMinEE()
			lastminEE = minEE

			//Update EE and minEE if possible only when local EE is increased
			if getEE(Lpayload, sf, tpindex, RealTxpowerArrayWatt[tpindex], AverageSNR, ED, Msf) > EE[ED] {

				EE[ED] = getEE(Lpayload, sf, tpindex, RealTxpowerArrayWatt[tpindex], AverageSNR, ED, Msf)
				getRealM()

				//Get current minEE
				getMinEE()

				sfAssigned[ED] = sf
				tpAssigned[ED] = float64(tpindex)

				//fmt.Printf("Inter EE: %f\n\n", EE)

				drAssigned[ED] = 12 - sfAssigned[ED]

				//fmt.Printf("minEE-lastminEE: %f\n", minEE-lastminEE)
				//Convergence condition based on threshold
				if minEE-lastminEE <= threshold {
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

					return //no need to send Mac command even all 48 combinations are traversed
				}

			}
		}

	}
	//Only DyLoRa will reach the end of code normally
	SNRGain[ED] = 0.0

}
