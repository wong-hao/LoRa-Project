package src

import (
	"fmt"
	"math"
)

const (
	maxTxPower    = 19
	txPowerOffset = 2
	minTxPower    = maxTxPower - txPowerOffset*7

	npreamble = 8    //Length in symbol
	nsync     = 4.25 //Length in symbol
	nheader   = 8    //Length in symbol
	Lcrc      = 2    //Length in bit
	CRC       = 1
	DE        = 1
	IH        = 0

	RateCode = 0.8
	CR       = (4 * (1 - RateCode)) / RateCode
	BW       = 125000
)

var (
	Ps                 [M][N]float64 //Symbol error rate in PolarScheduler not bit error rate in DyLoRa
	Ppreamble          [M][N]float64
	Pheader            [M][N]float64
	Ppayload           [M][N]float64
	npayload           float64 //PHYpayload length in symbols
	Pnc                [M][N]float64
	Pc                 [M]float64
	PDR                [M][N]float64
	PER                [M]float64
	PRR                [M]float64
	Throughput         [M]float64
	InstantPER         [M]float64
	InstantPRR         [M]float64 //Instant Packet Reception Ratio with time
	EE                 [M]float64 //bit/mJ
	EEb                float64    // energy candidate
	EEdelta            = 0.0      //EEdelta is negative: the greater the gap, the more unreliable
	minEE              = 0.0      //Although almost all energy efficiency is not influenced by ED number, but the minimal one is influenced by ED number
	lastexecutionminEE = 0.0      // minimal energy efficiency at last execution
	minEEdelta         = 0.0
	lastroundminEE     = 0.0 // minimal energy efficiency at last round
	threshold          = 0.01
	loopcount          = 0.0

	sfAssigned  [M]float64
	tpAssigned  [M]float64
	tpExisting  [M]float64 //Already assigned txpower, in other words, the existing txpower at last round
	drAssigned  [M]float64
	sfExisiting [M]float64 //Only for co-SF interference

	TxpowerArray = [...]float64{maxTxPower, maxTxPower - txPowerOffset, maxTxPower - txPowerOffset*2, maxTxPower - txPowerOffset*3, maxTxPower - txPowerOffset*4, maxTxPower - txPowerOffset*5, maxTxPower - txPowerOffset*6, minTxPower}
	//TxpowerArrayWatt = [...]float64{1139.0, 902.0, 850.0, 603.0, 556.0, 500.0, 470.0, 435.0} //Sx1276+Arduino MilliWatt （Measured at 5V）
	TxpowerArrayWatt = [...]float64{950.0, 872.0, 750.0, 603.0, 556.0, 500.0, 470.0, 435.0} //Sx1276+Arduino MilliWatt （Measured at 5V）

	Msf     = 0        //使用相同SF的节点个数
	SNRGain [M]float64 //Ideal change
	//SNRGainTable = [...]float64{0.0, -1.4, -2.8, -4.1, -5.0, -6.1, -8.1, -9.2} // SNR gain in dB with TP change
	SNRGainTable = [...]float64{0.0, -1.4, -2.8, -4.1, -5.0, -6.1, -7.3, -8.1} // SNR gain in dB with TP change
	RealSNRGain  [M]float64                                                    //Diminishing increment, which onsiders the fact that the txpower of the node can not be really changed
)

// https://www.rapidtables.com/convert/power/dBm_to_Watt.html
// Change the array to Sx1276 MilliWatt （Defined in regional specification）
func dBm2milliWatt(output *[8]float64) {
	for i, m := range TxpowerArray {
		compound1 := m - 30
		compound2 := compound1 / 10
		compound3 := math.Pow(10, compound2)
		output[i] = 1000 * compound3
	}
}

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

// Get random SNR [offset1, input1+offset1) + [offset2, input2+offset2)/10
func getRandomSNR(input1 int, offset1 int, input2 int, offset2 int) float64 {
	IntegerPart := getRandomFloat(input1, offset1)
	FractionPart := getRandomFloat(input2, offset2)
	return IntegerPart + 0.1*FractionPart
}

// Array of average SNR of recent messages for recent messages with node ED: each element corresponds to a single GW
func getAverageSNR(AverageSNR *[M][N]float64) {
	var sumM float64

	for k := 0; k < N; k++ {

		sumM = 0.0

		for _, m := range uplinkSNRHistory[ED][k] {
			sumM += m
		}

		uplinkSNRHistoryLen[ED] = float64(len(uplinkSNRHistory[ED][k]))

		AverageSNR[ED][k] = sumM / uplinkSNRHistoryLen[ED]
	}

	fmt.Printf("AverageSNR: %v\n", AverageSNR)
}

func CalculateTPGain(tpIndex int) {
	SNRGain[ED] = SNRGainTable[tpIndex] - SNRGainTable[int(tpExisting[ED])]
}

// Get SNR gain according to the txpower （Only called at each runtime combination and not influence the real SNR value statistics)
func getSNRGain(tpIndex int, GW int, AverageSNR *[M][N]float64) {
	CalculateTPGain(tpIndex)
	AverageSNR[ED][GW] = AverageSNR[ED][GW] + SNRGain[ED]
}

// Q https://stackoverflow.com/questions/56075838/how-to-generate-the-values-for-the-q-function
// https://www.mathworks.com/help/comm/ref/qfunc.html
func Q(intput float64) float64 {
	return 0.5 - 0.5*math.Erf(intput/math.Sqrt(2))
}

func getPs(sf float64, AverageSNR float64) float64 {
	compound1 := math.Pow(10, AverageSNR/10.0)
	//fmt.Printf("compound1: %f ", compound1)
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
	compound1 := sf + math.Log2(npreamble+nsync)
	return 1 - getPs(compound1, AverageSNR)
}

func getPheader(Ps float64) float64 {
	compound1 := math.Pow(1-Ps, 4)
	compound2 := 3 * math.Pow(1-Ps, 7) * Ps
	compound3 := compound1 + compound2
	compound4 := math.Ceil(nheader / 4) //与ADR中Nstep的向下取整相反
	return math.Pow(compound3, compound4)
}

func getPpayload(Ps float64, Lpayload float64, sf float64) float64 {
	compound1 := 1 - Ps
	//More accurate calculation
	npayload = getnPayload(sf, Lpayload)
	compound2 := math.Ceil(npayload - nheader)
	//As DyloRa
	//compound2 := math.Ceil(Lpayload / sf)
	return math.Pow(compound1, compound2)
}

func getPnc(Ppreamble float64, Pheader float64, Ppayload float64) float64 {
	return Ppreamble * Pheader * Ppayload
}

func getRb(sf float64) float64 {
	return (sf * RateCode) / (math.Pow(2, sf+1) / BW)
}

// Get packet bit length
func getL(sf float64, Lpayload float64) float64 {
	return sf*(npreamble+nsync+nheader) + Lpayload + Lcrc
}

// Get time on air from data bit rate
func getToABit(L float64, Rb float64) float64 {
	return L / Rb
}

// Get estimated duty cycle
func getAlpha(T float64) float64 {
	return T * Lambda
}

func getG(a float64, Msf int) float64 {
	return a * float64(Msf)
}

func getPc(sf float64, Lpayload float64, Msf int) float64 {
	//Original wrong idea
	//Rb := getRb(sf)
	//L := getL(sf, Lpayload)
	//T := getToABit(L, Rb)
	//From EFLoRa
	T := getToASymble(sf, Lpayload)
	a := getAlpha(T)
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

func getPER(ED int) { //https://github.com/brocaar/chirpstack-network-server/blob/4e7fdb348b5d465c8faacbf6a1f6f5fabea88066/internal/adr/default.go#L137
	//ATTENTION: 与网关处的Packet error ratio“相比，会出现网关没有全部收到正确数据包，就没有将其转发给NS，导致计算公式中的间隔出现错误，而这是无法解释的（吞吐数据量同理），所以这个指标最好不用于有效性的解释而是优越性
	var lostPackets int
	var previousFCnt int
	var length float64

	for i, m := range UplinkFcntHistorySlice[ED] {
		if i == 0 {
			previousFCnt = m
			continue
		}

		lostPackets += m - previousFCnt - 1 // there is always an expected difference of 1
		previousFCnt = m
	}

	length = float64(UplinkFcntHistorySlice[ED][len(UplinkFcntHistorySlice[ED])-1] - 0 + 1)

	InstantPER[ED] = float64(lostPackets) / length
	InstantPRR[ED] = 1 - InstantPER[ED]

}

func getRs(sf float64) float64 {
	return BW / math.Pow(2, sf)
}

func getTs(sf float64) float64 {
	return 1 / getRs(sf)
}
func getTpreamble(sf float64) float64 {
	Ts := getTs(sf)
	return (npreamble + nsync) * Ts
}

// Get payload plus header symbol number  (attention: nsymbol is not linear with SF)
func getnPayload(sf float64, Lpayload float64) float64 {
	compound1 := Lpayload - 4*sf + 28 + 16*CRC - 20*IH
	compound2 := 4.0 * (sf - 2*DE)
	compound3 := math.Ceil(compound1 / compound2)
	compound4 := compound3 * (CR + 4)
	compound5 := 0.0
	if compound4 > 0 {
		compound5 = compound4
	}
	compound6 := 8.0 + compound5
	return compound6
}

func getTpayload(sf float64, Lpayload float64) float64 {
	npayload = getnPayload(sf, Lpayload)
	Ts := getTs(sf)
	return npayload * Ts
}

// Get time on air from data symbol rate
// Calculator: https://www.rfwireless-world.com/calculators/LoRaWAN-Airtime-calculator.html /https://avbentem.github.io/airtime-calculator/ttn/cn470/15
func getToASymble(sf float64, Lpayload float64) float64 {
	Tpreamble := getTpreamble(sf)
	Tpayload := getTpayload(sf, Lpayload)
	Tpacket := Tpreamble + Tpayload
	return Tpacket
}

func getAlgorithmRuntime() {
	AlgorithmRuntime = 1000 * AlgorithmSnaptime.Sub(AlgorithmInitTime).Seconds()
	fmt.Printf("INFO: [up] Algorithm run time use in %f ms\n", AlgorithmRuntime)
}

// Reset
func ResetSNRArray(ED int) {
	num[ED] = 0
	for i := 0; i < N; i++ {
		uplinkSNRHistory[ED][i] = uplinkSNRHistory[ED][i][0:0]
	}
}
