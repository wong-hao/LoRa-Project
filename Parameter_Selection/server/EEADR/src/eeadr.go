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

	AlgorithmInitTime time.Time
	AlgorithmSnaptime time.Time
	AlgorithmRuntime  float64
)

const (
	InitialTemperature = 5.0 //When too low, it is hard to set a high packet reception ratio
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
	//fmt.Printf("Metropolis: %v\n", Metropolis)
}

func SimulatedAnnealing(Lpayload float64, ED int) {

	AlgorithmInitTime = time.Now()

	fmt.Printf("Lpayload: %f\n", Lpayload)

	//Get SNR considering TP gain
	var AverageSNR [M][N]float64
	tpExisting[ED] = tpAssigned[ED]
	getAverageSNR(&AverageSNR)

	//Combination algorithm

	loopcount = 0

	//初始解：与设备初次发送的参数相同
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
			EEb = getEE(Lpayload, sf, tpindex, TxpowerArrayWatt[tpindex], AverageSNR, ED, Msf)
			getMinEE()
			lastminEE = minEE

			getRealM()

			//Perturbation
			sfp, tpindexp := Perturbation(int(math.Pow(T0, float64(MarkovChainLen))), T0, sf, tpindex)
			loopcount++

			//Get current minEE
			getMsf(sfp)
			Msfp := Msf

			//Get f(x')
			EEp := getEE(Lpayload, sfp, tpindexp, TxpowerArrayWatt[tpindexp], AverageSNR, ED, Msfp)
			getMinEE()

			//Judge whether to accept the new solution
			if EEp > EEb {
				fmt.Printf("Something is wrong1: EEafter: %f, EEbefore: %f\n", EE[ED], EEb)
				sf = sfp
				tpindex = tpindexp
			} else {
				getMetropolis(EEp, EEb, T0, ED)
				if rand.Float64() < Metropolis[ED] && (minEE-lastminEE) >= threshold { //Metropolis criterion that makes sure the perturbed solution will not influence the global minimal EE
					fmt.Printf("Something is wrong2: EEafter: %f, EEbefore: %f\n", EE[ED], EEb)
					sf = sfp
					tpindex = tpindexp
				}
			}

			// Get the real collied result
			getMsf(sf)

			EE[ED] = getEE(Lpayload, sf, tpindex, TxpowerArrayWatt[tpindex], AverageSNR, ED, Msf)

			getMinEE()

			sfAssigned[ED] = sf
			tpAssigned[ED] = float64(tpindex)

			drAssigned[ED] = 12 - sfAssigned[ED]

		}
	}

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
}
