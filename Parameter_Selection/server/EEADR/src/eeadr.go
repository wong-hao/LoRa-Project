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
	Metropolis    [M]float64
)

const (
	InitialTemperature = 3.0
	MinimumTemperature = 1
	ChianLength        = 1
	Alpha              = 0.95
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

	//Get SNR considering TP gain
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
					Debuginfo(ED)
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

		getMsf(sfb)

		//Only to reduce the transmission power
		for tpindexb := 0; tpindexb <= len(TxpowerArray)-1; tpindexb++ {

			loopcount++

			EEb := getEE(Lpayload, sfb, tpindexb, TxpowerArrayWatt[tpindexb], AverageSNR, ED, Msf)

			//Update EE and minEE if possible only when local EE is increased
			if EEb >= maxEE {
				maxEE = EEb

				sf = sfb
				tpindex = tpindexb
			}
		}
	}

	getMsf(sf)

	EE[ED] = getEE(Lpayload, sf, tpindex, TxpowerArrayWatt[tpindex], AverageSNR, ED, Msf)

	getRealM()

	getMinEE()

	sfAssigned[ED] = sf
	tpAssigned[ED] = float64(tpindex)

	drAssigned[ED] = 12 - sfAssigned[ED]

	printStatistic()
	Debuginfo(ED)
	logData(ED)

	GrpcAllocation(int(drAssigned[ED]), int(tpAssigned[ED]), 1, ED)

	num[ED] = 0
	for i := 0; i < N; i++ {
		uplinkSNRHistory[ED][i] = uplinkSNRHistory[ED][i][0:0]
	}

}

func Perturbation(randseed int, T0 float64, sf float64, tpindex int) (float64, int) {

	rand.Seed(int64(2*randseed+1) * time.Now().UnixNano())

	PerturbedSf := 0.0
	PerturbedTpindex := 0

	for {
		//https://studygolang.com/topics/2733
		PerturbedSf = sf + float64(getRandomInt(5, -2))
		PerturbedTpindex = tpindex + getRandomInt(5, -2)

		if PerturbedSf >= 7 && PerturbedSf <= 12 && PerturbedTpindex >= 0 && PerturbedTpindex <= 7 {
			break
		}
	}

	return PerturbedSf, PerturbedTpindex

}

func getMetropolis(new float64, old float64, T float64, ED int) {
	dE := new - old //negative number: the greater the gap, the more unreliable
	k := minEE / 10
	//Cooling is the same as increasing interval difference: reduce acceptance probability
	Metropolis[ED] = math.Exp(dE / k * T) //f(x) = e^x, x<0
	fmt.Printf("Metropolis: %v\n", Metropolis)
}

func SimulatedAnnealing(Lpayload float64, ED int) {

	fmt.Printf("Lpayload: %f\n", Lpayload)

	//Get SNR considering TP gain
	var AverageSNR [M][N]float64
	tpExisting[ED] = tpAssigned[ED]
	getAverageSNR(&AverageSNR)

	//Combination algorithm

	loopcount = 0

	//初始解：设定为设备初次发送的参数
	sf := 12.0
	tpindex := 0
	if sfAssigned[ED] != 0.0 || drAssigned[ED] != 0 || tpAssigned[ED] != 0 {
		sf = sfAssigned[ED]
		tpindex = int(tpAssigned[ED])
	}

	for T0 := InitialTemperature; T0 > MinimumTemperature; T0 *= Alpha {

		for MarkovChainLen := 0; MarkovChainLen < ChianLength; MarkovChainLen++ {

			//Get last minEE
			getMsf(sf)

			//Get f(x)
			EE[ED] = getEE(Lpayload, sf, tpindex, TxpowerArrayWatt[tpindex], AverageSNR, ED, Msf)
			EEb := EE[ED]
			getMinEE()
			minEEb := minEE

			getRealM()

			//Perturbation
			sfp, tpindexp := Perturbation(int(math.Pow(T0, float64(MarkovChainLen))), T0, sf, tpindex)
			loopcount++

			//Get current minEE
			getMsf(sfp)
			Msfp := Msf

			//Get f(x')
			EE[ED] = getEE(Lpayload, sfp, tpindexp, TxpowerArrayWatt[tpindexp], AverageSNR, ED, Msfp)
			EEp := EE[ED]
			getMinEE()
			minEEp := minEE

			//Judge whether to accept the new solution
			if EEp > EEb {
				fmt.Printf("Something is wrong1: EEafter: %f, EEbefore: %f\n", EEp, EEb)
				sf = sfp
				tpindex = tpindexp
			} else {
				getMetropolis(EEp, EEb, T0, ED)
				if rand.Float64() < Metropolis[ED] && minEEb < minEEp { //Metropolis criterion
					fmt.Printf("Something is wrong2: EEafter: %f, EEbefore: %f\n", EEp, EEb)
					sf = sfp
					tpindex = tpindexp
				}
			}

			getMsf(sf)
			EE[ED] = getEE(Lpayload, sf, tpindex, TxpowerArrayWatt[tpindex], AverageSNR, ED, Msf)
			getMinEE()

			sfAssigned[ED] = sf
			tpAssigned[ED] = float64(tpindex)

			drAssigned[ED] = 12 - sfAssigned[ED]

		}
	}

	printStatistic()
	Debuginfo(ED)
	logData(ED)

	GrpcAllocation(int(drAssigned[ED]), int(tpAssigned[ED]), 1, ED)

	num[ED] = 0
	for i := 0; i < N; i++ {
		uplinkSNRHistory[ED][i] = uplinkSNRHistory[ED][i][0:0]
	}
}

func ILS(Lpayload float64, ED int) {
	/*
		//sf, tpIndex := HillClimbing(Lpayload, ED, -1, -1)
		sf, tpIndex := SimulatedAnnealing(Lpayload, ED)

		if sf == -1 && tpIndex == -1 {
			return
		}

		sfAssigned[ED] = sf
		tpAssigned[ED] = float64(tpIndex)

		//fmt.Printf("Inter EE: %f\n\n", EE)

		drAssigned[ED] = 12 - sfAssigned[ED]
		//fmt.Printf("Metropolis: %v\n", Metropolis)

		printStatistic()
		Debuginfo()
		logData(ED)

		GrpcAllocation(int(drAssigned[ED]), int(tpAssigned[ED]), 1, ED)

	*/
}
