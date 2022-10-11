package src

import (
	"fmt"
	"math"
)

var (
	RealMLocation []int   //The index slice of non-zero EE
	RealM         float64 //The number of non-zero EE
)

// RemoveRepeatedElement https://www.jianshu.com/p/e6dd5c3591c2
func RemoveRepeatedElement(arr []int) (newArr []int) {
	newArr = make([]int, 0)
	for i := 0; i < len(arr); i++ {
		repeat := false
		for j := i + 1; j < len(arr); j++ {
			if arr[i] == arr[j] {
				repeat = true
				break
			}
		}
		if !repeat {
			newArr = append(newArr, arr[i])
		}
	}
	return
}

// Get the number and location of real M
func getRealM() {
	RealM = 0.0

	for i, j := range EE {
		if math.Abs(j) >= 1e-6 {
			RealM++

			RealMLocation = append(RealMLocation, i)
			RealMLocation = RemoveRepeatedElement(RealMLocation)
		}
	}
}

func getMinEE() {

	EEM := 99999.0
	for _, m := range RealMLocation {
		if EE[m] < EEM {
			EEM = EE[m]
		}
	}
	minEE = EEM
}

func EEADR(Lpayload float64, ED int) {
	fmt.Printf("Lpayload: %f\n", Lpayload)

	var AverageSNR [M][N]float64

	tpExisting[ED] = tpAssigned[ED]
	getAverageSNR(&AverageSNR)

	//Combination algorithm

	//Only try to increase the data rate to guarantee the fine-grained operations

	loopcount = 0

	for sf := 12.0; sf >= 7.0; sf-- {

		getMsf(sf)

		//Only to reduce the transmission power
		for tpindex, tp := range TxpowerArrayWatt {

			loopcount++

			//Get last minEE
			getMinEE()
			lastminEE = minEE

			//Update EE and minEE if possible only when local EE is increased
			if getEE(Lpayload, sf, tpindex, tp, AverageSNR, ED, Msf) > EE[ED] {

				EE[ED] = getEE(Lpayload, sf, tpindex, tp, AverageSNR, ED, Msf)
				getRealM()

				//Get current minEE
				getMinEE()

				sfAssigned[ED] = sf
				tpAssigned[ED] = float64(tpindex)

				//fmt.Printf("Inter EE: %f\n\n", EE)

				drAssigned[ED] = 12 - sfAssigned[ED]

				if DyLoRa == false {
					//fmt.Printf("minEE-lastminEE: %f\n", minEE-lastminEE)
					//Convergence condition based on threshold
					if minEE-lastminEE <= threshold {
						printStatistic()
						Debuginfo()
						logData(ED)

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

		GrpcAllocation(int(drAssigned[ED]), int(tpAssigned[ED]), 1, ED)
	} else {
		//Only DyLoRa will reach the end of code normally
		SNRGain[ED] = 0.0
	}
}
