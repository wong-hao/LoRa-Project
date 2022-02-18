package src

import (
	"fmt"
	"math"
)

const (
	maxTxPower    = 19
	minTxPower    = maxTxPower - txPowerOffset*7
	txPowerOffset = 2
)

var (
	AverageSNR [M][N]float64
	Psymbol    [M][N]float64
	Ppreamble  [M][N]float64
	Pheader    [M][N]float64
	Ppayload   [M][N]float64
	PRR        [M][N]float64
	Pcollision [M]float64
	PER        = [M]float64{1.0, 1.0}
	PDR        [M]float64
	EE         = [M]float64{0.0, 0.0}
	sfAssigned [M]float64
	tpAssigned [M]float64
	drAssigned [M]float64

	TxpowerArray     = [...]float64{maxTxPower, maxTxPower - txPowerOffset, maxTxPower - txPowerOffset*2, maxTxPower - txPowerOffset*3, maxTxPower - txPowerOffset*4, maxTxPower - txPowerOffset*5, maxTxPower - txPowerOffset*6, minTxPower}
	TxpowerArrayWatt [8]float64
	SfArray          = [...]float64{7.0, 8.0, 9.0, 10.0, 11.0, 12.0}
)

func dB2Watt(input [8]float64, output *[8]float64) {
	for i, m := range input {
		output[i] = 1000 * math.Pow(10, (m-30)/10)

	}
}

func getAverageSNR(array [HISTORYCOUNT]float64) float64 {
	var snrM float64
	var sumM = 0.0

	for _, m := range array {
		sumM += m

	}
	snrM = sumM / HISTORYCOUNT
	return snrM
}

// Q https://stackoverflow.com/questions/56075838/how-to-generate-the-values-for-the-q-function
func Q(intput float64) float64 {
	return 0.5 - 0.5*math.Erf(intput/math.Sqrt(2))
}

func getPsymble(sf float64, AverageSNR float64) float64 {
	return 0.5 * Q(math.Sqrt(math.Pow(10, AverageSNR/10.0)*math.Pow(2, sf+1))-math.Sqrt(1.386*sf+1.154))
}

func getPreamble(sf float64, AverageSNR float64) float64 {
	return 1 - getPsymble(sf+math.Log2(12.5), AverageSNR)
}

func getPheader(Psymbol float64) float64 {
	return math.Pow(math.Pow(1-Psymbol, 4)+3*math.Pow(1-Psymbol, 7)*Psymbol, 8)
}

func getPpayload(Psymbol float64, Lpayload float64, sf float64) float64 {
	return math.Pow(1-Psymbol, Lpayload/sf)
}

func getPRR(Ppreamble float64, Pheader float64, Ppayload float64) float64 {
	return Ppreamble * Pheader * Ppayload
}

func getPcollision(sf float64, Lpayload float64) float64 {
	return 1 - math.Exp((-1)*(math.Pow(2, sf+1)/sf)*(sf*20.25+Lpayload+2/100000)*M*(M/20))
}

func getEE(Lpayload float64, sf float64, tp float64) float64 {
	for k := 0; k < N; k++ {
		AverageSNR[0][k] = getAverageSNR(uplinkSNRHistory[0][k])
		Psymbol[0][k] = getPsymble(sf, AverageSNR[0][k])
		Ppreamble[0][k] = getPreamble(sf, AverageSNR[0][k])
		Pheader[0][k] = getPheader(Psymbol[0][k])
		Ppayload[0][k] = getPpayload(Psymbol[0][k], Lpayload, sf)
		PRR[0][k] = getPRR(Ppreamble[0][k], Pheader[0][k], Ppayload[0][k])
		Pcollision[0] = getPcollision(sf, Lpayload)
		PRR[0][k] = PRR[0][k] * (1 - Pcollision[0])
		PER[0] = PER[0] * (1 - PRR[0][k])
	}

	PDR[0] = 1 - PER[0]

	/*
		fmt.Printf("AverageSNR: %v\n", AverageSNR)
		fmt.Printf("Psymbol: %v\n", Psymbol)
		fmt.Printf("Ppreamble: %v\n", Ppreamble)
		fmt.Printf("Pheader: %v\n", Pheader)
		fmt.Printf("Ppayload: %v\n", Ppayload)
		fmt.Printf("Pcollision: %f\n", Pcollision)
		fmt.Printf("PRR: %f\n", PRR)
		fmt.Printf("PER: %f\n", PER)
		fmt.Printf("PDR: %f\n", PDR)
	*/

	return (sf * 125000 * PDR[0]) / (math.Pow(2, sf) * tp)
}

func getCombination(Lpayload float64) {
	fmt.Printf("Lpayload: %f\n", Lpayload)
	fmt.Printf("TxpowerArrayWatt: %v\n", TxpowerArrayWatt)

	for _, sf := range SfArray {
		for j, tp := range TxpowerArrayWatt {
			if getEE(Lpayload, sf, tp) > EE[0] {
				EE[0] = getEE(Lpayload, sf, tp)
				sfAssigned[0] = sf
				tpAssigned[0] = float64(j)
			}
		}
	}

	drAssigned[0] = 12 - sfAssigned[0]

	fmt.Printf("EE: %f\n", EE)
	fmt.Printf("sfAssigned: %f\n", sfAssigned)
	fmt.Printf("drAssigned: %f\n", drAssigned)
	fmt.Printf("tpAssigned: %f\n", tpAssigned)
}
