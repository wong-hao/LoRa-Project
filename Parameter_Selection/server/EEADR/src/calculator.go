package src

import (
	"fmt"
	"os"
)

// Just for experimental debug purpose

func calculateEE(sf float64, tpindex int, ED int, GWNum int) {
	AverageSNR := [M][N]float64{
		{-5},
		{-5},
	}

	if GWNum != N {
		fmt.Printf("Please set the correct GW number!\n")
		os.Exit(1)
	}

	EE[ED] = getEE(224, sf, tpindex, TxpowerArrayWatt[tpindex], AverageSNR, ED, 0)

	fmt.Printf("EE: %v\n", EE)
	Debuginfo(ED)

}

func calculateEE2(Lpayload float64, PRR float64, sf float64, tpIndex int) {
	compound1 := Lpayload * PRR
	compound2 := getToASymble(sf, Lpayload) * RealTxpowerArrayWatt[tpIndex]
	fmt.Printf("Calcuated EE: %f\n", compound1/compound2)
}
