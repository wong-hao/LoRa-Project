package src

import (
	"math"
)

func getPropagation(Lpayload float64, sf float64, AverageSNR [M][N]float64) {
	for k := 0; k < N; k++ {
		Ps[ED][k] = getPs(sf, AverageSNR[ED][k])
		Ppreamble[ED][k] = getPreamble(sf, AverageSNR[ED][k])
		Pheader[ED][k] = getPheader(Ps[ED][k])
		Ppayload[ED][k] = getPpayload(Ps[ED][k], Lpayload, sf)
		Pnc[ED][k] = getPnc(Ppreamble[ED][k], Pheader[ED][k], Ppayload[ED][k])
	}
}

func getCollision(Lpayload float64, sf float64, Msf int) {
	for k := 0; k < N; k++ {
		Pc[ED] = getPc(sf, Lpayload, Msf)
	}
}

func getLink() {
	for k := 0; k < N; k++ {
		PDR[ED][k] = Pnc[ED][k] * (1 - Pc[ED])
	}
}

func getMultiple() {
	PER[ED] = 1.0 //Initializaion

	for k := 0; k < N; k++ {
		PER[ED] = PER[ED] * (1 - PDR[ED][k])
		PRR[ED] = 1 - PER[ED]
	}
}

func getEE(Lpayload float64, sf float64, tp float64, AverageSNR [M][N]float64, ED int, Msf int) float64 {

	getPropagation(Lpayload, sf, AverageSNR)
	getCollision(Lpayload, sf, Msf)
	getLink()
	getMultiple()

	/*
		fmt.Printf("Ps: %v\n", Ps)
		fmt.Printf("Ppreamble: %v\n", Ppreamble)
		fmt.Printf("Pheader: %v\n", Pheader)
		fmt.Printf("Ppayload: %v\n", Ppayload)
		fmt.Printf("Pcollision: %f\n", Pc)
		fmt.Printf("PRR: %f\n", PRR)
		fmt.Printf("PER: %f\n", PER)
		fmt.Printf("PDR: %f\n", PDR)
	*/

	compound1 := sf * BW * PRR[ED] * RateCode
	compound2 := math.Pow(2, sf) * tp
	ee := compound1 / compound2
	//fmt.Printf("ee: %f\n\n", ee)

	return ee
}
