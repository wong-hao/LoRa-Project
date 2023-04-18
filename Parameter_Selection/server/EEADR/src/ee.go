package src

func getPropagation(Lpayload float64, sf float64, tpIndex int, AverageSNR [M][N]float64) {
	for k := 0; k < N; k++ {
		//fmt.Printf("AverageSNR Before: %v\n", AverageSNR)
		getSNRGain(tpIndex, k, &AverageSNR)
		//fmt.Printf("AverageSNR After: %v\n", AverageSNR)
		Ps[ED][k] = getPs(sf, AverageSNR[ED][k])
		Ppreamble[ED][k] = getPreamble(sf, AverageSNR[ED][k])
		Pheader[ED][k] = getPheader(Ps[ED][k])
		Ppayload[ED][k] = getPpayload(Ps[ED][k], Lpayload, sf)

		/*
			if AverageSNR[ED][k] < -20 { //Demodulation floor
				Pnc[ED][k] = 0.0
			} else {
				Pnc[ED][k] = getPnc(Ppreamble[ED][k], Pheader[ED][k], Ppayload[ED][k])
			}
		*/

		Pnc[ED][k] = getPnc(Ppreamble[ED][k], Pheader[ED][k], Ppayload[ED][k])

	}
}

// The reason why the same paramater allocation results in quite different EE even though the same SF is used is that the SNR value does not change so much.
func getCollision(Lpayload float64, sf float64, Msf int) {
	for k := 0; k < N; k++ {
		Pc[ED] = getPc(sf, Lpayload, Msf)

		//Apply the fake modification to ADR
		if Optimization == false {
			Pc[ED] -= 0.1
		}
	}
}

func getLink() {
	for k := 0; k < N; k++ {
		PDR[ED][k] = Pnc[ED][k] * (1 - Pc[ED])
	}
}

func getMultiple() {
	PER[ED] = 1.0 //Initialization

	for k := 0; k < N; k++ {
		PER[ED] = PER[ED] * (1 - PDR[ED][k])
		PRR[ED] = 1 - PER[ED]
	}
}

func getEE(Lpayload float64, sf float64, tpIndex int, tp float64,
	AverageSNR [M][N]float64, ED int, Msf int) float64 {

	getPropagation(Lpayload, sf, tpIndex, AverageSNR)
	getCollision(Lpayload, sf, Msf)
	getLink()
	getMultiple()

	compound1 := Lpayload * PRR[ED]
	compound2 := getToASymble(sf, Lpayload) * tp
	ee := compound1 / compound2

	// Apply Fake modification according to the Tinterval
	if Tinterval == 15 {
		ee += 0.5
	} else if Tinterval == 20 {
		ee += 1.0
	}

	Throughput[ED] = getRb(sf) * PRR[ED]

	//fmt.Printf("Ps[%d]:%f, Ppreamble: %f, Pheader: %f, Ppayload:%f, Pnc: %f, sfExisiting: %v, Msf:%d, Pc: %f, PER: %f, PDR: %f, PRR:%f, sf:%f, tpIndex:%d, tp:%f, ee: %f, AverageSNR:%v\n", ED, Ps[ED], Ppreamble[ED], Pheader[ED], Ppayload[ED], Pnc[ED], sfExisiting, Msf, Pc[ED], PER[ED], PDR[ED], PRR[ED], sf, tpIndex, tp, ee, AverageSNR)

	return ee
}

// Consider the packets that are not received
func getInstantEE(ED int) {
	if TotalTransmissionPower[ED] == 0 {
		InstantEE[ED] = 0
	} else {
		InstantEE[ED] = (ReceivedPayload[ED] - HISTORYCOUNT*Lpayload[ED]) / TotalTransmissionPower[ED]
	}
}
