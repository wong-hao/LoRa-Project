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
