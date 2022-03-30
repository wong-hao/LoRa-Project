package src

import (
	"fmt"
)

func getMinEE(EE [M]float64) float64 {
	snrM := 99999.0
	for _, m := range EE {
		if m < snrM {
			snrM = m
		}
	}
	return snrM
}

func EEADR(Lpayload float64, ED int) {
	fmt.Printf("Lpayload: %f\n", Lpayload)

	for k := 0; k < N; k++ {
		AverageSNR[ED][k] = getAverageSNR(uplinkSNRHistory[ED][k])
	}
	fmt.Printf("AverageSNR: %v\n", AverageSNR)

	//do-while: https://golangtc.com/t/55eaf182b09ecc478200006e; https://www.jianshu.com/p/2ac52fe2810e
	for {
		loopcount++
		fmt.Printf("Loopcount: %d\n", loopcount)

		lastminEE = minEE
		fmt.Printf("Inter lastminEE: %f\n", lastminEE)

		//Combination algorithm
		for _, sf := range SfArray {
			for j, tp := range TxpowerArrayWatt {
				if getEE(Lpayload, sf, tp, AverageSNR, ED) > EE[ED] {
					EE[ED] = getEE(Lpayload, sf, tp, AverageSNR, ED)
					sfAssigned[ED] = sf
					tpAssigned[ED] = float64(j)
				}
			}
		}
		fmt.Printf("Inter EE: %f\n\n", EE)

		drAssigned[ED] = 12 - sfAssigned[ED]

		minEE = getMinEE(EE)

		fmt.Printf("minEE: %f\n", minEE)

		//fmt.Printf("minEE-lastminEE: %f\n", minEE-lastminEE)

		//Convergence condition based on threshold
		if minEE-lastminEE <= threshold {
			fmt.Printf("sfAssigned: %f\n", sfAssigned)
			fmt.Printf("drAssigned: %f\n", drAssigned)
			fmt.Printf("tpAssigned: %f\n", tpAssigned)
			fmt.Printf("Final EE: %f\n", EE)
			fmt.Printf("Jain's fairness index: %f\n\n", getFairness(EE))
			logData(1000*SnapshotTime.Sub(InitTime).Seconds(), ED, EE, getFairness(EE))

			//GrpcAllocation(int(drAssigned[ED]), int(tpAssigned[ED]), 1, ED)

			break
		}
	}

}
