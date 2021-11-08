package src

import "fmt"

const (
	margin_db     = 10
	maxDR         = 5
	maxTxPower    = 19.15
	minTxPower    = maxTxPower - txPowerOffset*7
	txPowerOffset = 2
)

var (
	RequiredSNRForDR      float64
	RequiredSNRForDRArray = [...]float64{-20, -17.5, -15, -12.5, -10, -7.5}

	snrMargin    float64
	nStep        int
	txPowerIndex int
	TxpowerArray = [...]float64{maxTxPower, maxTxPower - txPowerOffset, maxTxPower - txPowerOffset*2, maxTxPower - txPowerOffset*3, maxTxPower - txPowerOffset*4, maxTxPower - txPowerOffset*5, maxTxPower - txPowerOffset*6, minTxPower}

	pktLossRate float64
)

func defalutADR(dr int, txPower *float64, nbTrans *int) {

	for i, j := range RequiredSNRForDRArray {
		if dr == i {
			RequiredSNRForDR = j
		}
	}

	snrMargin = getMaxSNR(uplinkSNRHistory) - RequiredSNRForDR - margin_db
	//snrMargin = getAverageSNR(uplinkSNRHistory)-RequiredSNRForDR - margin_db

	pktLossRate = getPacketLossPercentage(uplinkFcntHistory)
	fmt.Printf("pktLossRate: %f%%\n", pktLossRate)

	// Set the new NbTrans.
	*nbTrans = getNbTrans(*nbTrans, pktLossRate)

	nStep = int(snrMargin / 3)

	dr, txPowerIndex = getIdealTxPowerIndexAndDR(nStep, txPower, dr)

	//TODO: 看network-server的configuration里的disable_mac_commands=true是否会禁止ADR
	//disable_adr=true或者disable_mac_commands=true后仍可以通过grpc发送MAC command
	//TODO: 看confirmed message的确认是否会停止ADR设置的重传
	//TODO: 看未设置ADR时confirmed message未收到ACK会不会导致重传 (实验中似乎出现过)，属于ARQ
	GRPC_Allocation(dr, txPowerIndex, *nbTrans)

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

func getIdealTxPowerIndexAndDR(nStep int, txPower *float64, dr int) (int, int) {

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

	return dr, txPowerIndex

}

func getPacketLossPercentage(array [HISTORYCOUNT]int) float64 {
	var lostPackets int
	var previousFCnt int
	var len float64

	for i, m := range array {
		if i == 0 {
			previousFCnt = m
			continue
		}

		lostPackets += m - previousFCnt - 1 // there is always an expected difference of 1
		previousFCnt = m
	}

	len = float64(array[HISTORYCOUNT-1] - array[0] + 1)

	return float64(lostPackets) / len * 100
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

	if pktLossRate < 5 {
		return pktLossRateTable()[0][currentNbTrans-1]
	} else if pktLossRate < 10 {
		return pktLossRateTable()[1][currentNbTrans-1]
	} else if pktLossRate < 30 {
		return pktLossRateTable()[2][currentNbTrans-1]
	}

	return pktLossRateTable()[3][currentNbTrans-1]

}

func testADR(num1 int, num2 *float64) {
	*num2 = maxTxPower - float64(num1-6)*txPowerOffset
	for i, j := range TxpowerArray {
		if *num2 == j {
			txPowerIndex = i
		}
	}
	GRPC_Allocation(num1, txPowerIndex, 1)
}
