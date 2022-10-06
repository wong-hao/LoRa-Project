package src

import (
	"math/rand"
	"time"
)

// Get random number http://t.csdn.cn/VnjcA
func getRandom(input int) int {
	rand.Seed(time.Now().UnixNano())
	return -(rand.Intn(input) + 10)
}
