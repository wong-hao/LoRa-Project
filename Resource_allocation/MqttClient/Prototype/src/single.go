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
	AverageSNR [N]float64
	Psymbol    [N]float64
	Ppreamble  [N]float64
	Pheader    [N]float64

	TxpowerArray     = [...]float64{maxTxPower, maxTxPower - txPowerOffset, maxTxPower - txPowerOffset*2, maxTxPower - txPowerOffset*3, maxTxPower - txPowerOffset*4, maxTxPower - txPowerOffset*5, maxTxPower - txPowerOffset*6, minTxPower}
	TxpowerArrayWatt [8]float64
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

func getPheader(Psymbol float64, sf float64) float64 {
	return math.Pow(math.Pow(1-Psymbol, 4)+3*math.Pow(1-Psymbol, 7)*Psymbol, math.Ceil(2.0/sf))
}

func single(sf float64, Lpayload float64) {
	for i := 0; i < N; i++ {
		AverageSNR[i] = getAverageSNR(uplinkSNRHistory[i])
		Psymbol[i] = getPsymble(sf, AverageSNR[i])
		Ppreamble[i] = getPsymble(sf+math.Log2(12.5), AverageSNR[i])
		Pheader[i] = getPheader(Psymbol[i], sf)
	}

	fmt.Printf("AverageSNR: %v\n", AverageSNR)
	fmt.Printf("Psymbol: %v\n", Psymbol)
	fmt.Printf("Ppreamble: %v\n", Psymbol)
	fmt.Printf("Pheader: %v\n", Psymbol)
	fmt.Printf("Lpayload: %f\n\n", Lpayload)
	fmt.Printf("TxpowerArrayWatt: %v\n", TxpowerArrayWatt)

}
