package src

import (
	"fmt"
	"math"
)

var (
	AverageSNR [N]float64
	Psymbol    [N]float64
)

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

func single(sf float64) {
	for i := 0; i < N; i++ {
		AverageSNR[i] = getAverageSNR(uplinkSNRHistory[i])
		Psymbol[i] = 0.5 * Q(math.Sqrt(math.Pow(10, AverageSNR[i]/10.0)*math.Pow(2, sf+1))-math.Sqrt(1.386*sf+1.154))
	}

	fmt.Printf("AverageSNR: %v\n", AverageSNR)
	fmt.Printf("Psymbol: %v\n\n", Psymbol)

}
