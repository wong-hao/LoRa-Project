package main

import "fmt"

const(
	initialnum = 5
)
var(
	test1  = initialnum
	test2 = [...]float64{19.15, 17.15, 15.15, 13.15, 11.15, 9.15, 7.15, 5.15}

)

func main() {
	fmt.Printf("test1: %d\n",test1)
	changeNum(&test1)
	fmt.Printf("test1: %d\n",test1)
	changeNum(&test1)
	fmt.Printf("test1: %d\n",test1)
	fmt.Printf("test2: %f\n",test2[0])
	changeLastArrayItem(&test2[0])
	fmt.Printf("test2: %f\n",test2[0])
	changeLastArrayItem(&test2[0])
	fmt.Printf("test2: %f\n",test2[0])
}

func changeNum(num *int)  {
	*num++
}

func changeLastArrayItem(num *float64)  {
	*num++

}