package main

import "fmt"

var(
	slice = make([]string, 0, 100)
 errornum int = 0
	MICErrorPercentage int
	slicelen int
	bb float64

)

func main()  {



	slice = append(slice, "sad")
	slice = append(slice, "sad")


	fmt.Printf("Len of slice: %d\n", len(slice))

	for _, v := range slice {
		if v == "sad" {
			errornum++
		}

	}
	slicelen = len(slice)

	bb = float64(errornum) / float64(slicelen)

	fmt.Printf("MIC error percentage: %f\n", bb)
	fmt.Printf("Num of error: %d\n", errornum)

	fmt.Printf("%v\n", slice)

	fmt.Printf("%f", (errornum*1.0)/len(slice))
}