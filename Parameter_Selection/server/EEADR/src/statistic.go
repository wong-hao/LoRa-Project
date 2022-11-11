package src

import "fmt"

func printStatistic() {
	fmt.Printf("/* ------------------------------Static info begins------------------------------------------- */\n")
	getTotalTime()
	fmt.Printf("Loopcount of ED %d: %f\n", ED, loopcount)

	fmt.Printf("sfAssigned: %f\n", sfAssigned)
	fmt.Printf("drAssigned: %f\n", drAssigned)
	fmt.Printf("tpAssigned: %f\n", tpAssigned)
	fmt.Printf("Final EE: %f\n", EE)
	getFairness()
	fmt.Printf("/* ------------------------------Static info ends------------------------------------------- */\n")
}

func Debuginfo(ED int) {
	fmt.Printf("/* ------------------------------Debug info begins------------------------------------------- */\n")
	fmt.Printf("Ps[%d]: %v\n", ED, Ps[ED])
	fmt.Printf("Ppreamble[%d]: %v\n", ED, Ppreamble[ED])
	fmt.Printf("Pheader[%d]: %v\n", ED, Pheader[ED])
	fmt.Printf("Ppayload[%d]: %v\n", ED, Ppayload[ED])
	fmt.Printf("Single Not Pnc[%d]: %v\n", ED, Pnc[ED])
	fmt.Printf("Existing SF: %f\n", sfExisiting)
	fmt.Printf("Msf: %d\n", Msf)
	fmt.Printf("Pcollision[%d]: %f\n", ED, Pc[ED])
	fmt.Printf("PDR[%d]: %f\n", ED, PDR[ED])
	fmt.Printf("PER[%d]: %f\n", ED, PER[ED])
	fmt.Printf("PRR[%d]: %f\n", ED, PRR[ED])
	getAverageEE(ED)
	fmt.Printf("/* ------------------------------Debug info ends------------------------------------------- */\n")
}
