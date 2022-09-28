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

func Debuginfo() {
	fmt.Printf("/* ------------------------------Debug info begins------------------------------------------- */\n")
	fmt.Printf("Ps: %v\n", Ps)
	fmt.Printf("Ppreamble: %v\n", Ppreamble)
	fmt.Printf("Pheader: %v\n", Pheader)
	fmt.Printf("Ppayload: %v\n", Ppayload)
	fmt.Printf("Single Not Pnc: %v\n", Pnc)
	fmt.Printf("Existing SF: %f\n", sfExisiting)
	fmt.Printf("Msf: %d\n", Msf)
	fmt.Printf("Pcollision: %f\n", Pc)
	fmt.Printf("PDR: %f\n", PDR)
	fmt.Printf("PER: %f\n", PER)
	fmt.Printf("PRR: %f\n", PRR)
	fmt.Printf("/* ------------------------------Debug info ends------------------------------------------- */\n")
}
