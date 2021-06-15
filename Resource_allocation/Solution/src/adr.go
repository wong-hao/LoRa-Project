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

func defalutADR(num1 int, num2 *float64, num3 *int)  {

	for i, j := range RequiredSNRForDRArray {
		if num1 == i {
			RequiredSNRForDR = j
		}
	}

	snrMargin = getMaxSNR(uplinkSNRHistory)-RequiredSNRForDR - margin_db
	//snrMargin = getAverageSNR(uplinkSNRHistory)-RequiredSNRForDR - margin_db

	pktLossRate = getPacketLossPercentage(uplinkFcntHistory)

	// Set the new NbTrans.
	*num3 = getNbTrans(*num3,pktLossRate)

	nStep = int(snrMargin/3)

	for {
		if nStep == 0 {
			break
		} else if nStep > 0 {
			if num1 < maxDR {
				num1++
			} else {
				*num2 = *num2 - txPowerOffset
			}
			for i, j := range TxpowerArray {
				if *num2 == j {
					txPowerIndex = i
				}
			}
			GRPC_Allocation(num1, txPowerIndex, *num3)
			nStep--
			if *num2 == minTxPower {
				return
			}
		} else if nStep < 0 {
			if *num2 < maxTxPower {
				*num2 = *num2 + txPowerOffset
			} else {
				return
			}
			for i, j := range TxpowerArray {
				if *num2 == j {
					txPowerIndex = i
				}
			}
			GRPC_Allocation(num1, txPowerIndex, *num3)
			nStep++
		}
	}

}

func testADR(num1 int, num2 *float64)  {
	*num2 = maxTxPower - float64(num1-6)*txPowerOffset
	for i, j := range TxpowerArray {
		if *num2 == j {
			txPowerIndex = i
		}
	}
	GRPC_Allocation(num1, txPowerIndex,1)
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