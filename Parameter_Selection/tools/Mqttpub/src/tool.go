package src

import "math/rand"

// Get random number [offset, n+offset) http://t.csdn.cn/VnjcA
func getRandomInt(input int, offset int) int {
	Randomrange := rand.Intn(input)
	HigerRange := Randomrange + offset
	IntRange := HigerRange
	return IntRange
}

// Get random number [offset, n+offset)
func getRandomFloat(input int, offset int) float64 {
	Randomrange := rand.Intn(input)
	HigerRange := Randomrange + offset
	FloatRange := float64(HigerRange)
	return FloatRange
}

func getRandomSNR(input1 int, offset1 int, input2 int, offset2 int) float64 {
	IntegerPart := getRandomFloat(input1, offset1)
	FractionPart := getRandomFloat(input2, offset2)
	return IntegerPart + 0.1*FractionPart
}
