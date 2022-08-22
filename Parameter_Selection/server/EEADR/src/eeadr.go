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

	getAverageSNR()
	fmt.Printf("AverageSNR: %v\n", AverageSNR)

	//Combination algorithm
	for _, sf := range SfArray {

		Ns = 0
		for _, m := range sfExisiting {
			if sf == m {
				Ns++
			}
		}

		for j, tp := range TxpowerArrayWatt {

			loopcount++

			//Get current minEE
			getMinEE()
			lastminEE = minEE

			//Update EE and minEE(if possible)
			if getEE(Lpayload, sf, tp, AverageSNR, ED, Ns) > EE[ED] {
				EE[ED] = getEE(Lpayload, sf, tp, AverageSNR, ED, Ns)
				getMinEE()

				sfAssigned[ED] = sf
				tpAssigned[ED] = float64(j)

				fmt.Printf("Inter EE: %f\n\n", EE)

				drAssigned[ED] = 12 - sfAssigned[ED]
			}

			//fmt.Printf("minEE-lastminEE: %f\n", minEE-lastminEE)
			//Convergence condition based on threshold
			if (minEE != 0 && minEE-lastminEE <= threshold) || (int(loopcount) == len(SfArray)*len(TxpowerArray)) { //minEE == 0 means every device just gets the maximal value at the first loop without greedy algorithm
				fmt.Printf("/* ------------------------------Static info begins------------------------------------------- */\n")
				getTotalTime()
				fmt.Printf("Loopcount: %f\n", loopcount)

				fmt.Printf("sfAssigned: %f\n", sfAssigned)
				fmt.Printf("drAssigned: %f\n", drAssigned)
				fmt.Printf("tpAssigned: %f\n", tpAssigned)
				fmt.Printf("Final EE: %f\n", EE)
				getFairness()
				fmt.Printf("/* ------------------------------Static info ends------------------------------------------- */\n")

				logData(ED)
				loopcount = 0

				//GrpcAllocation(int(drAssigned[ED]), int(tpAssigned[ED]), 1, ED)

				return
			}
		}
	}
}
