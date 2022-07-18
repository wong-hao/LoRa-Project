package src

import (
	"math"
)

const (
	maxTxPower    = 19
	minTxPower    = maxTxPower - txPowerOffset*7
	txPowerOffset = 2

	Lpreamble = 8    //Symble length
	LSync     = 4.25 //Symble length
	Lheader   = 8    //Symble length
	Lcrc      = 2    //Bit length

	RateCode = 0.8
	BW       = 125000
)

var (
	AverageSNR [M][N]float64
	Psymbol    [M][N]float64
	Ppreamble  [M][N]float64
	Pheader    [M][N]float64
	Ppayload   [M][N]float64
	PRR        [M][N]float64
	Pcollision [M]float64
	PER        = [M]float64{1.0, 1.0}
	PDR        [M]float64
	EE         = [M]float64{0.0, 0.0} //bit/mJ
	minEE      = 0.0
	lastminEE  float64
	threshold  = 0.01
	loopcount  = 0 //run time count variable

	sfAssigned  [M]float64
	tpAssigned  [M]float64
	drAssigned  [M]float64
	sfExisiting [M]float64 //已有的扩频因子：用于计算co-SF interference

	TxpowerArray     = [...]float64{maxTxPower, maxTxPower - txPowerOffset, maxTxPower - txPowerOffset*2, maxTxPower - txPowerOffset*3, maxTxPower - txPowerOffset*4, maxTxPower - txPowerOffset*5, maxTxPower - txPowerOffset*6, minTxPower}
	TxpowerArrayWatt [8]float64 //MilliWatt
	SfArray          = [...]float64{7.0, 8.0, 9.0, 10.0, 11.0, 12.0}

	Ns = 0 //使用相同SF的节点个数
)

//https://www.rapidtables.com/convert/power/dBm_to_Watt.html
func dBm2milliWatt(output *[8]float64) {
	for i, m := range TxpowerArray {
		compound1 := m - 30
		compound2 := compound1 / 10
		compound3 := math.Pow(10, compound2)
		output[i] = 1000 * compound3
	}
}

func getAverageSNR() {
	var sumM float64

	for k := 0; k < N; k++ {

		sumM = 0.0

		for _, m := range uplinkSNRHistory[ED][k] {
			sumM += m

		}

		AverageSNR[ED][k] = sumM / HISTORYCOUNT
	}
}

// Q https://stackoverflow.com/questions/56075838/how-to-generate-the-values-for-the-q-function
func Q(intput float64) float64 {
	return 0.5 - 0.5*math.Erf(intput/math.Sqrt(2))
}

func getPsymble(sf float64, AverageSNR float64) float64 {
	compound1 := math.Pow(10, AverageSNR/10.0)
	compound2 := math.Pow(2, sf+1)
	compound3 := compound1 * compound2
	compound4 := 1.386*sf + 1.154
	compound5 := math.Sqrt(compound3)
	compound6 := math.Sqrt(compound4)
	compound7 := compound5 - compound6
	return 0.5 * Q(compound7)
}

func getPreamble(sf float64, AverageSNR float64) float64 {
	compound1 := sf + math.Log2(Lpreamble+LSync)
	return 1 - getPsymble(compound1, AverageSNR)
}

func getPheader(Psymbol float64) float64 {
	compound1 := math.Pow(1-Psymbol, 4)
	compound2 := 3 * math.Pow(1-Psymbol, 7) * Psymbol
	compound3 := compound1 + compound2
	compound4 := math.Ceil(Lheader / 4) //与ADR中Nstep的向下取整相反
	return math.Pow(compound3, compound4)
}

func getPpayload(Psymbol float64, Lpayload float64, sf float64) float64 {
	compound1 := 1 - Psymbol
	compound2 := math.Ceil(Lpayload / sf)
	return math.Pow(compound1, compound2)
}

func getPRR(Ppreamble float64, Pheader float64, Ppayload float64) float64 {
	return Ppreamble * Pheader * Ppayload
}

func getPcollision(sf float64, Lpayload float64, Ns int) float64 {
	compound1 := math.Pow(2, sf+1) / sf
	compound2 := sf*(Lpreamble+LSync+Lheader) + Lpayload + Lcrc
	compound3 := BW * RateCode
	compound4 := compound2 / compound3
	compound5 := float64(Ns) / float64(Tinterval)
	compound6 := (-1) * compound1 * compound4 * compound5
	return 1 - math.Exp(compound6)
}

func getEE(Lpayload float64, sf float64, tp float64, AverageSNR [M][N]float64, ED int, Ns int) float64 {

	for k := 0; k < N; k++ {
		Psymbol[ED][k] = getPsymble(sf, AverageSNR[ED][k])
		Ppreamble[ED][k] = getPreamble(sf, AverageSNR[ED][k])
		Pheader[ED][k] = getPheader(Psymbol[ED][k])
		Ppayload[ED][k] = getPpayload(Psymbol[ED][k], Lpayload, sf)
		PRR[ED][k] = getPRR(Ppreamble[ED][k], Pheader[ED][k], Ppayload[ED][k])
		Pcollision[ED] = getPcollision(sf, Lpayload, Ns)
		PRR[ED][k] = PRR[ED][k] * (1 - Pcollision[ED])
		PER[ED] = PER[ED] * (1 - PRR[ED][k])
		PDR[ED] = 1 - PER[ED]

	}

	/*
		fmt.Printf("Psymbol: %v\n", Psymbol)
		fmt.Printf("Ppreamble: %v\n", Ppreamble)
		fmt.Printf("Pheader: %v\n", Pheader)
		fmt.Printf("Ppayload: %v\n", Ppayload)
		fmt.Printf("Pcollision: %f\n", Pcollision)
		fmt.Printf("PRR: %f\n", PRR)
		fmt.Printf("PER: %f\n", PER)
		fmt.Printf("PDR: %f\n", PDR)
	*/

	compound1 := sf * BW * PDR[ED] * RateCode
	compound2 := math.Pow(2, sf) * tp
	ee := compound1 / compound2
	//fmt.Printf("ee: %f\n\n", ee)

	return ee
}
