package src

import "fmt"

// Just for experimental debug purpose

func calculateEE(sf float64, tpindex int, ED int) {
	AverageSNR := [M][N]float64{
		{-10},
		{-10},
		{-10},
	}

	fmt.Printf("TestEE: %f\n", getEE(224, sf, tpindex, TxpowerArrayWatt[tpindex], AverageSNR, ED, 0))
	fmt.Printf("TestPs: %f\n", getPs(sf, AverageSNR[ED][0]))
	fmt.Printf("TestPRR: %v\n", PRR)

}
