package src

const (
	margin_db = 10
	maxDR = 5
	maxTxPower = 19.15
	minTxPower = maxTxPower - txPowerOffset*7
	txPowerOffset = 2
)

var (

	RequiredSNRForDR float64
	RequiredSNRForDRArray = [...]float64{-20,-17.5,-15,-12.5,-10,-7.5}

	snrMargin float64
	nStep int
	txPowerIndex int
	TxpowerArray = [...]float64{maxTxPower, maxTxPower-txPowerOffset, maxTxPower-txPowerOffset*2, maxTxPower-txPowerOffset*3, maxTxPower-txPowerOffset*4, maxTxPower-txPowerOffset*5, maxTxPower-txPowerOffset*6, minTxPower}

	pktLossRate float64

)

func defalutADR(dr int, txPower *float64, nbTrans *int)  {

	for i, j := range RequiredSNRForDRArray {
		if dr == i {
			RequiredSNRForDR = j
		}
	}

	snrMargin = getMaxSNR(uplinkSNRHistory)-RequiredSNRForDR - margin_db
	//snrMargin = getAverageSNR(uplinkSNRHistory)-RequiredSNRForDR - margin_db

	pktLossRate = getPacketLossPercentage(uplinkFcntHistory)

	// Set the new NbTrans.
	*nbTrans = getNbTrans(*nbTrans,pktLossRate)

	nStep = int(snrMargin/3)

	dr, txPowerIndex = getIdealTxPowerIndexAndDR(nStep,txPower,dr)


}


func getMaxSNR(array [HISTORYCOUNT]float64) float64 {
	var snrM float64 = -999
	for _, m := range array {
		if m > snrM {
			snrM = m
		}
	}
	return snrM
}

func getAverageSNR(array [HISTORYCOUNT]float64) float64 {
	var snrM float64
	var sumM = 0.0

	for _, m := range array {
		sumM += m

	}
	snrM = sumM / HISTORYCOUNT
	return snrM
}

func getIdealTxPowerIndexAndDR(nStep int, txPower *float64, dr int) (int,int)  {

	for {
		if nStep == 0 {
			break
		} else if nStep > 0 {
			if dr < maxDR {
				dr++
			} else {
				*txPower = *txPower - txPowerOffset
			}
			for i, j := range TxpowerArray {
				if *txPower == j {
					txPowerIndex = i
				}
			}
			nStep--
			if *txPower == minTxPower {
				break
			}
		} else if nStep < 0 {
			if *txPower < maxTxPower {
				*txPower = *txPower + txPowerOffset
			} else {
				break
			}
			for i, j := range TxpowerArray {
				if *txPower == j {
					txPowerIndex = i
				}
			}
			nStep++
		}
	}

	return dr,txPowerIndex

}

func getPacketLossPercentage(array [HISTORYCOUNT]int) float64 {
	var lostPackets int
	var previousFCnt int

	for i, m := range array {
		if i == 0 {
			previousFCnt = m
			continue
		}

		lostPackets += m - previousFCnt - 1 // there is always an expected difference of 1
		previousFCnt = m
	}

	return float64(lostPackets) / HISTORYCOUNT
}

func pktLossRateTable() [][3]int {
	return [][3]int{
		{1, 1, 2},
		{1, 2, 3},
		{2, 3, 3},
		{3, 3, 3},
	}
}
func getNbTrans(currentNbTrans int, pktLossRate float64) int {
	if currentNbTrans < 1 {
		currentNbTrans = 1
	}

	if currentNbTrans > 3 {
		currentNbTrans = 3
	}

	if pktLossRate < 0.05 {
		return pktLossRateTable()[0][currentNbTrans-1]
	} else if pktLossRate < 0.10 {
		return pktLossRateTable()[1][currentNbTrans-1]
	} else if pktLossRate < 0.30 {
		return pktLossRateTable()[2][currentNbTrans-1]
	}

	return pktLossRateTable()[3][currentNbTrans-1]

}