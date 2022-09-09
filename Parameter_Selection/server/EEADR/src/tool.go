package src

import "math"

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
	Pnc        [M][N]float64
	Pc         [M]float64
	PDR        [M][N]float64
	PER        = [M]float64{1.0, 1.0}
	PRR        [M]float64
	EE         = [M]float64{0.0, 0.0} //bit/mJ
	minEE      = 0.0
	lastminEE  float64
	threshold  = 0.01
	loopcount  = 0.0 //run time count variable

	sfAssigned  [M]float64
	tpAssigned  [M]float64
	drAssigned  [M]float64
	sfExisiting [M]float64 //已有的扩频因子：用于计算co-SF interference

	TxpowerArray     = [...]float64{maxTxPower, maxTxPower - txPowerOffset, maxTxPower - txPowerOffset*2, maxTxPower - txPowerOffset*3, maxTxPower - txPowerOffset*4, maxTxPower - txPowerOffset*5, maxTxPower - txPowerOffset*6, minTxPower}
	TxpowerArrayWatt [8]float64 //MilliWatt
	SfArray          = [...]float64{7.0, 8.0, 9.0, 10.0, 11.0, 12.0}

	Msf = 0 //使用相同SF的节点个数
)

// https://www.rapidtables.com/convert/power/dBm_to_Watt.html
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

func getPnc(Ppreamble float64, Pheader float64, Ppayload float64) float64 {
	return Ppreamble * Pheader * Ppayload
}

func getRb(sf float64) float64 {
	return (sf * RateCode) / (math.Pow(2, sf+1) / BW)
}

func getL(sf float64, Lpayload float64) float64 {
	return sf*(Lpreamble+LSync+Lheader) + Lpayload + Lcrc
}

func getT(L float64, Rb float64) float64 {
	return L / Rb
}

func geta(T float64) float64 {
	return T / float64(Tinterval)
}

func getG(a float64, Msf int) float64 {
	return a * float64(Msf)
}

func getPc(sf float64, Lpayload float64, Msf int) float64 {
	Rb := getRb(sf)
	L := getL(sf, Lpayload)
	T := getT(L, Rb)
	a := geta(T)
	G := getG(a, Msf)
	return 1 - math.Exp(-2*G)
}
