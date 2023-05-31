package src

import (
	"fmt"
	"math"
)

var (
	Fairness     float64
	RealMLen     int //Length of slice
	LastRealMLen int //Length of slice at Last round
)

func sum() float64 {
	Sum := 0.0
	for _, value := range EE {
		if math.Abs(value) >= 1e-6 {
			Sum += value
		}
	}
	return Sum
}

func sumofsquares() float64 {
	Sum := 0.0
	for _, value := range EE {
		if math.Abs(value) >= 1e-6 {
			Sum += math.Pow(value, 2)
		}
	}
	return Sum
}

func getFairness() {
	Sum1 := sum()
	molecule := math.Pow(Sum1, 2)
	Sum2 := sumofsquares()
	denominator := RealM * Sum2
	fmt.Printf("Sum1: %f\n", Sum1)
	fmt.Printf("Sum2: %f\n", Sum2)

	LastRealMLen = RealMLen
	RealMLen = len(RealMLocation)
	if LastRealMLen < RealMLen {
		Fairness = math.NaN()
	} else {
		Fairness = molecule / denominator
	}

	fmt.Printf("EE: %v\n", EE)
	fmt.Printf("molecule: %f\n", molecule)
	fmt.Printf("denominator: %f\n", denominator)
	fmt.Printf("RealMLen: %d\n", RealMLen)
	fmt.Printf("Jain's fairness index: %f\n", Fairness)
}
