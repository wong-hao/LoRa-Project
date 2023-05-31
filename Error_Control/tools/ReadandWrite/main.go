package main

import (
	"ReadandWrite/src"
	"fmt"
)

func main() {

	if src.Optimization == true {
		if src.SOTA == true {
			src.OptimizationName = "OPR"
		} else {
			src.OptimizationName = "ReLoRaWAN"
		}
	} else {
		src.OptimizationName = "Off"
	}

	if src.Optimization == true {
		if src.SOTA == true {
			fmt.Printf("OPR On!\n")
		} else {
			fmt.Printf("ReLoRaWAN On!\n")
		}
	} else {
		fmt.Printf("ADR On!\n")
	}

	// 每隔Tinterval秒读取本级子目录bin下的第1个csv文件的最后一行数据，并将其中的第2列与第8列数据存储到变量里
	src.ReadLastLineFromCSVInDir(0, 1, 2, 3, 8, src.Tinterval)
}
