package src

import (
	"fmt"
	"math"
	"math/rand"
)

const (
	maxTxPower    = 19
	minTxPower    = maxTxPower - txPowerOffset*7
	txPowerOffset = 2

	Lpreamble = 8    //Length in symbol
	LSync     = 4.25 //Length in symbol
	Lheader   = 8    //Length in symbol
	Lcrc      = 2    //Length in bit

	RateCode = 0.8
	BW       = 125000
)

var (
	Ps        [M][N]float64 //Symbol error rate in PolarScheduler not bit error rate in DyLoRa
	Ppreamble [M][N]float64
	Pheader   [M][N]float64
	Ppayload  [M][N]float64
	Pnc       [M][N]float64
	Pc        [M]float64
	PDR       [M][N]float64
	PER       [M]float64
	PRR       [M]float64
	EE        [M]float64 //bit/mJ
	minEE     = 0.0
	lastminEE = 0.0
	threshold = 0.01
	loopcount = 0.0

	sfAssigned  [M]float64
	tpAssigned  [M]float64
	tpExisting  [M]float64 //Already assigned txpower, in other words, the existing txpower at last round
	drAssigned  [M]float64
	sfExisiting [M]float64 //Only for co-SF interference

	TxpowerArray     = [...]float64{maxTxPower, maxTxPower - txPowerOffset, maxTxPower - txPowerOffset*2, maxTxPower - txPowerOffset*3, maxTxPower - txPowerOffset*4, maxTxPower - txPowerOffset*5, maxTxPower - txPowerOffset*6, minTxPower}
	TxpowerArrayWatt [8]float64 //MilliWatt

	Msf         = 0        //使用相同SF的节点个数
	SNRGain     [M]float64 //Ideal change
	RealSNRGain [M]float64 //Diminishing increment, which onsiders the fact that the txpower of the node can not be really changed
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

func getAverageSNR(AverageSNR *[M][N]float64) { //Average SNR of node ED for recent HISTORYCOUNT num messages
	var sumM float64

	for k := 0; k < N; k++ {

		sumM = 0.0

		for _, m := range uplinkSNRHistory[ED][k] {
			sumM += m

		}

		AverageSNR[ED][k] = sumM / HISTORYCOUNT
	}

	fmt.Printf("AverageSNR: %v\n", AverageSNR)
}

func CalculateTPGain(tpIndex int) {
	gap := tpExisting[ED] - float64(tpIndex)
	SNRGain[ED] = txPowerOffset * gap
}

// Get SNR gain according to the txpower （Only called at each runtime combination and not influence the real SNR value statistics)
func getSNRGain(tpIndex int, AverageSNR *[M][N]float64) {
	for k := 0; k < N; k++ {
		CalculateTPGain(tpIndex)
		AverageSNR[ED][k] = AverageSNR[ED][k] + SNRGain[ED]
	}
}

// Q https://stackoverflow.com/questions/56075838/how-to-generate-the-values-for-the-q-function
// https://www.mathworks.com/help/comm/ref/qfunc.html
func Q(intput float64) float64 {
	return 0.5 - 0.5*math.Erf(intput/math.Sqrt(2))
}

func getPs(sf float64, AverageSNR float64) float64 {
	compound1 := math.Pow(10, AverageSNR/10.0)
	compound2 := math.Pow(2, sf+1)
	compound3 := compound1 * compound2
	compound4 := 1.386*sf + 1.154
	compound5 := math.Sqrt(compound3)
	compound6 := math.Sqrt(compound4)
	compound7 := compound5 - compound6
	compound8 := Q(compound7)
	return 0.5 * compound8
}

func getPreamble(sf float64, AverageSNR float64) float64 {
	compound1 := sf + math.Log2(Lpreamble+LSync)
	return 1 - getPs(compound1, AverageSNR)
}

func getPheader(Ps float64) float64 {
	compound1 := math.Pow(1-Ps, 4)
	compound2 := 3 * math.Pow(1-Ps, 7) * Ps
	compound3 := compound1 + compound2
	compound4 := math.Ceil(Lheader / 4) //与ADR中Nstep的向下取整相反
	return math.Pow(compound3, compound4)
}

func getPpayload(Ps float64, Lpayload float64, sf float64) float64 {
	compound1 := 1 - Ps
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

func getMsf(sf float64) { //计算使用相同SF的节点个数（不考虑未来分配的，只考虑目前环境中存在的）
	Msf = 0
	for _, m := range sfExisiting {
		if sf == m {
			Msf++
		}
	}
}
