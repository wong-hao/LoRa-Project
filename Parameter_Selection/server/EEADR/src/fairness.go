package src

import "math"

var (
	Fairness float64
)

func sum() float64 {
	Sum := 0.0
	for _, value := range EE {
		Sum += value
	}
	return Sum
}

func sumofsquares() float64 {
	Sum := 0.0
	for _, value := range EE {
		Sum += math.Pow(value, 2)
	}
	return Sum
}

func getFairness() {
	denominator := math.Pow(sum(), 2)
	molecule := M * sumofsquares()

	Fairness = denominator / molecule
}
