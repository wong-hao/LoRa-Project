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

	//do-while: https://golangtc.com/t/55eaf182b09ecc478200006e; https://www.jianshu.com/p/2ac52fe2810e
	for {
		loopcount++
		fmt.Printf("Loopcount: %d\n", loopcount)

		lastminEE = minEE
		fmt.Printf("Inter lastminEE: %f\n", lastminEE)

		//Combination algorithm
		for _, sf := range SfArray {

			Ns = 0
			for _, m := range sfExisiting {
				if sf == m {
					Ns++
				}
			}

			for j, tp := range TxpowerArrayWatt {
				if getEE(Lpayload, sf, tp, AverageSNR, ED, Ns) > EE[ED] {
					EE[ED] = getEE(Lpayload, sf, tp, AverageSNR, ED, Ns)
					sfAssigned[ED] = sf
					tpAssigned[ED] = float64(j)
				}
			}
		}
		fmt.Printf("Inter EE: %f\n\n", EE)

		drAssigned[ED] = 12 - sfAssigned[ED]

		getMinEE()

		fmt.Printf("minEE: %f\n", minEE)

		//fmt.Printf("minEE-lastminEE: %f\n", minEE-lastminEE)

		//Convergence condition based on threshold
		if minEE-lastminEE <= threshold {
			fmt.Printf("/* ------------------------------Static info begins------------------------------------------- */\n")
			getTotalTime()
			fmt.Printf("sfAssigned: %f\n", sfAssigned)
			fmt.Printf("drAssigned: %f\n", drAssigned)
			fmt.Printf("tpAssigned: %f\n", tpAssigned)
			fmt.Printf("Final EE: %f\n", EE)
			getFairness()
			fmt.Printf("/* ------------------------------Static info ends------------------------------------------- */\n")

			logData(ED)

			//GrpcAllocation(int(drAssigned[ED]), int(tpAssigned[ED]), 1, ED)

			break
		}
	}

}
