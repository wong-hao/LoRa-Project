package src

import "fmt"

func printStatistic() {
	fmt.Printf("/* ------------------------------Static info begins------------------------------------------- */\n")
	getTotalTime()
	fmt.Printf("FCNT: %d\n", fcnt) //Only for debuging
	getThroughput()
	fmt.Printf("UplinkFcntHistory: %v\n", UplinkFcntHistorySlice)
	getPER()
	fmt.Printf("/* ------------------------------Static info ends------------------------------------------- */\n")

}
