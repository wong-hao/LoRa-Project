package src

import "math"

var (
	Fairness float64
)

func sum(EE [M]float64) float64 {
	Sum := 0.0
	for _, value := range EE {
		Sum += value
	}
	return Sum
}

func sumofsquares(EE [M]float64) float64 {
	Sum := 0.0
	for _, value := range EE {
		Sum += math.Pow(value, 2)
	}
	return Sum
}
func getFairness(EE [M]float64) float64 {
	denominator := math.Pow(sum(EE), 2)
	molecule := M * sumofsquares(EE)

	Fairness = denominator / molecule
	return Fairness
}
