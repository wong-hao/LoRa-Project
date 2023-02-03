package src

import "fmt"

func printStatistic(ED int) {
	fmt.Printf("/* ------------------------------Static info begins------------------------------------------- */\n")
	getTotalTime()
	fmt.Printf("FCNT: %v\n", fcnt) //Only for debuging
	getThroughput(ED)
	fmt.Printf("UplinkFcntHistory: %v\n", UplinkFcntHistorySlice)
	getPER(ED)
	fmt.Printf("/* ------------------------------Static info ends------------------------------------------- */\n")
}
