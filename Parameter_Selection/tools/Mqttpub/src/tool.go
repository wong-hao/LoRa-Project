package src

import "math/rand"

// Get random number http://t.csdn.cn/VnjcA
func getRandomNum(input int, offset int) float64 {
	Randomrange := rand.Intn(input)
	HigerRange := Randomrange + offset
	FloatRange := float64(HigerRange)
	MinusRange := FloatRange
	return MinusRange
}
