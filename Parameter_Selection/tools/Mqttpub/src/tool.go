package src

import "math/rand"

// Get random number http://t.csdn.cn/VnjcA
func getRandomNum(input int, offset int) float64 {
	Randomrange := rand.Intn(input)
	HigerRange := Randomrange + offset
	FloatRange := float64(HigerRange)
	return FloatRange
}

func getRandomSNR(input1 int, offset1 int, input2 int, offset2 int) float64 {
	IntegerPart := getRandomNum(input1, offset1)
	FractionPart := getRandomNum(input2, offset2)
	return IntegerPart + 0.1*FractionPart
}
