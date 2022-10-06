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
	molecule := math.Pow(sum(), 2)
	denominator := RealM * sumofsquares()

	LastRealMLen = RealMLen
	RealMLen = len(RealMLocation)
	if LastRealMLen < RealMLen {
		Fairness = math.NaN()
	} else {
		Fairness = molecule / denominator
	}

	fmt.Printf("Jain's fairness index: %f\n", Fairness)
}
