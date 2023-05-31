package src

// Get random number [offset, input+offset) http://t.csdn.cn/VnjcA
func getRandomInt(input int, offset int) int {
	SmallRange := R.Intn(input)
	HugeRange := SmallRange + offset
	IntRange := HugeRange
	return IntRange
}

// Get random number [offset, input+offset)
func getRandomFloat(input int, offset int) float64 {
	SmallRange := R.Intn(input)
	HugeRange := SmallRange + offset
	FloatRange := float64(HugeRange)
	return FloatRange
}
