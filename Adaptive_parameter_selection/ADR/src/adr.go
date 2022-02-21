package src

const (
	margin_db     = 10
	maxDR         = 5
	maxTxPower    = 19
	minTxPower    = maxTxPower - txPowerOffset*7
	txPowerOffset = 2
)

var (
	RequiredSNRForDR      float64
	RequiredSNRForDRArray = [...]float64{-20, -17.5, -15, -12.5, -10, -7.5}

	snrMargin    float64
	MaxSNR       = -999.0
	nStep        int
	txPowerIndex int
	TxpowerArray = [...]float64{maxTxPower, maxTxPower - txPowerOffset, maxTxPower - txPowerOffset*2, maxTxPower - txPowerOffset*3, maxTxPower - txPowerOffset*4, maxTxPower - txPowerOffset*5, maxTxPower - txPowerOffset*6, minTxPower}
)

func ADR(dr int, txPower *float64, ED int) {

	for i, j := range RequiredSNRForDRArray {
		if dr == i {
			RequiredSNRForDR = j
		}
	}

	//Get max snr of all gateways
	for k := 0; k < N; k++ {
		if getMaxSNR(uplinkSNRHistory[ED][k]) > MaxSNR {
			MaxSNR = getMaxSNR(uplinkSNRHistory[ED][k])
		}
	}

	//fmt.Printf("MaxSNR: %f\n", MaxSNR)

	snrMargin = MaxSNR - RequiredSNRForDR - margin_db

	nStep = int(snrMargin / 3)

	dr, txPowerIndex = getIdealTxPowerIndexAndDR(nStep, txPower, dr)

	//TODO: 看network-server的configuration里的disable_mac_commands=true是否会禁止ADR
	//disable_adr=true或者disable_mac_commands=true后仍可以通过grpc发送MAC command
	//GrpcAllocation(dr, txPowerIndex, 1, ED)
}

//Get max snr of single gateway
func getMaxSNR(slice []float64) float64 {
	var snrM float64 = -99999
	for _, m := range slice {
		if m > snrM {
			snrM = m
		}
	}
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
