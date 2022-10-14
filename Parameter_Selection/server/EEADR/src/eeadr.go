package src

import (
	"fmt"
	"math"
	"math/rand"
	"time"
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
		for tpindex := 0; tpindex <= len(TxpowerArray)-1; tpindex++ {

			loopcount++

			//Get last minEE
			getMinEE()
			lastminEE = minEE

			//Update EE and minEE if possible only when local EE is increased
			if getEE(Lpayload, sf, tpindex, TxpowerArrayWatt[tpindex], AverageSNR, ED, Msf) > EE[ED] {

				EE[ED] = getEE(Lpayload, sf, tpindex, TxpowerArrayWatt[tpindex], AverageSNR, ED, Msf)
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
					printStatistic()
					Debuginfo()
					logData(ED)

					GrpcAllocation(int(drAssigned[ED]), int(tpAssigned[ED]), 1, ED)

					return //no need to send Mac command even all 48 combinations are traversed
				}

			}
		}

	}
	//Only DyLoRa will reach the end of code normally
	SNRGain[ED] = 0.0

}

func DyLoRa(Lpayload float64, ED int) {
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
		for tpindex := 0; tpindex <= len(TxpowerArray)-1; tpindex++ {

			loopcount++

			//Get last minEE
			getMinEE()
			lastminEE = minEE

			//Update EE and minEE if possible only when local EE is increased
			if getEE(Lpayload, sf, tpindex, TxpowerArrayWatt[tpindex], AverageSNR, ED, Msf) > EE[ED] {

				EE[ED] = getEE(Lpayload, sf, tpindex, TxpowerArrayWatt[tpindex], AverageSNR, ED, Msf)
				getRealM()

				//Get current minEE
				getMinEE()

				sfAssigned[ED] = sf
				tpAssigned[ED] = float64(tpindex)

				//fmt.Printf("Inter EE: %f\n\n", EE)

				drAssigned[ED] = 12 - sfAssigned[ED]

			}
		}

	}

	printStatistic()
	Debuginfo()
	logData(ED)

	GrpcAllocation(int(drAssigned[ED]), int(tpAssigned[ED]), 1, ED)

}
func HillClimbing(Lpayload float64, ED int, PerturbedSf float64, PerturbedTpindex int) (float64, int) {
	fmt.Printf("Lpayload: %f\n", Lpayload)

	var AverageSNR [M][N]float64

	tpExisting[ED] = tpAssigned[ED]
	getAverageSNR(&AverageSNR)

	//Combination algorithm

	//Only try to increase the data rate to guarantee the fine-grained operations

	loopcount = 0

	for sf := 12.0; sf >= 7.0; sf-- {

		if sf >= PerturbedSf {
			continue
		}

		getMsf(sf)

		//Only to reduce the transmission power
		for tpindex := 0; tpindex <= len(TxpowerArray)-1; tpindex++ {

			if tpindex <= PerturbedTpindex {
				continue
			}

			loopcount++

			//Get last minEE
			getMinEE()
			lastminEE = minEE

			//Update EE and minEE if possible only when local EE is increased
			if getEE(Lpayload, sf, tpindex, TxpowerArrayWatt[tpindex], AverageSNR, ED, Msf) > EE[ED] {

				EE[ED] = getEE(Lpayload, sf, tpindex, TxpowerArrayWatt[tpindex], AverageSNR, ED, Msf)
				getRealM()

				//Get current minEE
				getMinEE()

				//fmt.Printf("minEE-lastminEE: %f\n", minEE-lastminEE)
				//Convergence condition based on threshold
				if minEE-lastminEE <= threshold {
					return sf, tpindex //no need to send Mac command even all 48 combinations are traversed
				}
			}
		}
	}
	//Only DyLoRa will reach the end of code normally
	SNRGain[ED] = 0.0

	return -1, -1
}

func Perturbation(randseed int, T0 float64, sf float64, tpindex int) (float64, int) {

	rand.Seed(int64(2*randseed+1) * time.Now().UnixNano())

	PerturbatedSf := 0.0
	PerturbatedTpindex := 0

	for {
		//https://studygolang.com/topics/2733
		PerturbatedSf = sf + getRandomNum(13, -6)*(T0/10.0)
		PerturbatedTpindex = tpindex + int(getRandomNum(len(TxpowerArrayWatt)+1, -len(TxpowerArrayWatt)/2)*(T0/10.0))

		if PerturbatedSf >= 7 && PerturbatedSf <= 12 && PerturbatedTpindex >= 0 && PerturbatedTpindex <= 7 {
			break
		}
	}

	return PerturbatedSf, PerturbatedTpindex

}

func SimulatedAnnealing(Lpayload float64, ED int) (float64, int) {
	fmt.Printf("Lpayload: %f\n", Lpayload)

	var AverageSNR [M][N]float64

	tpExisting[ED] = tpAssigned[ED]
	getAverageSNR(&AverageSNR)

	//Combination algorithm

	//Only try to increase the data rate to guarantee the fine-grained operations

	loopcount = 0

	//初始解
	sf := 12.0
	tpindex := 0

	for T0 := 10.0; T0 > 1; T0 *= 0.95 {

		for MarkovChainLen := 0; MarkovChainLen < 1; MarkovChainLen++ {

			//Get last minEE
			getMsf(sf)
			Msfbefore := Msf
			EE[ED] = getEE(Lpayload, sf, tpindex, TxpowerArrayWatt[tpindex], AverageSNR, ED, Msfbefore)
			getMinEE()
			minEEbefore := minEE
			getRealM()

			//Perturbation
			sfp, tpindexp := Perturbation(int(math.Pow(T0, float64(MarkovChainLen))), T0, sf, tpindex)
			loopcount++

			//Get current minEE
			getMsf(sfp)
			Msfafter := Msf
			EE[ED] = getEE(Lpayload, sfp, tpindexp, TxpowerArrayWatt[tpindexp], AverageSNR, ED, Msfafter)
			getMinEE()
			minEEafter := minEE

			if minEEafter >= minEEbefore {
				sf = sfp
				tpindex = tpindexp

				Msf = Msfafter
				EE[ED] = minEEafter
				minEE = minEEafter
			} else if rand.Float64() < math.Exp(-(minEEafter-minEEbefore)/T0) { //Metropolis criterion
				Msf = Msfbefore
				EE[ED] = minEEbefore
				minEE = minEEbefore
			}
		}
	}

	CalculateTPGain(tpindex)

	return sf, tpindex
}

func ILS(Lpayload float64, ED int) {
	//sf, tpIndex := HillClimbing(Lpayload, ED, -1, -1)
	sf, tpIndex := SimulatedAnnealing(Lpayload, ED)

	if sf == -1 && tpIndex == -1 {
		return
	}

	sfAssigned[ED] = sf
	tpAssigned[ED] = float64(tpIndex)

	//fmt.Printf("Inter EE: %f\n\n", EE)

	drAssigned[ED] = 12 - sfAssigned[ED]

	printStatistic()
	Debuginfo()
	logData(ED)

	GrpcAllocation(int(drAssigned[ED]), int(tpAssigned[ED]), 1, ED)
}
