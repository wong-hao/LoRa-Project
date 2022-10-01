package src

import (
	"fmt"
)

func getMinEE() {
	snrM := 99999.0
	for _, m := range EE {
		if m < snrM {
			snrM = m
		}
	}
	minEE = snrM
}

func EEADR(Lpayload float64, ED int) {
	fmt.Printf("Lpayload: %f\n", Lpayload)

	var AverageSNR [M][N]float64

	getAverageSNR(&AverageSNR)

	//Combination algorithm

	//Only try to increase the data rate to guarantee the fine-grained operations
	for sf := 12.0; sf >= 7.0; sf-- {

		getMsf(sf)

		//Only to reduce the transmission power
		for j, tp := range TxpowerArrayWatt {

			loopcount++

			//Get current minEE
			getMinEE()
			lastminEE = minEE

			//Update EE and minEE if possible only when local EE is increased
			if getEE(Lpayload, sf, j, tp, AverageSNR, ED, Msf) > EE[ED] {

				EE[ED] = getEE(Lpayload, sf, j, tp, AverageSNR, ED, Msf)

				getMinEE()

				sfAssigned[ED] = sf
				tpAssigned[ED] = float64(j)

				//fmt.Printf("Inter EE: %f\n\n", EE)

				drAssigned[ED] = 12 - sfAssigned[ED]

				if DyLoRa == false {
					//fmt.Printf("minEE-lastminEE: %f\n", minEE-lastminEE)
					//Convergence condition based on threshold
					if minEE-lastminEE <= threshold {
						printStatistic()
						Debuginfo()
						logData(ED)
						loopcount = 0

						GrpcAllocation(int(drAssigned[ED]), int(tpAssigned[ED]), 1, ED)

						return //no need to send Mac command even all 48 combinations are traversed
					}
				}
			}
		}

	}

	if DyLoRa == true {
		printStatistic()
		Debuginfo()
		logData(ED)
		loopcount = 0

		GrpcAllocation(int(drAssigned[ED]), int(tpAssigned[ED]), 1, ED)
	}
}
