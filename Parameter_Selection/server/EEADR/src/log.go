package src

import (
	"encoding/csv"
	"fmt"
	"log"
	"os"
	"strconv"
	"time"
)

var (
	InitTime     = time.Now() // 获取当前时间
	SnapshotTime time.Time
	Totaltime    float64

	str      []string
	fileName = InitTime.Format("2006-01-02-15-04-05")
	fileType = "-Dataset.csv"
	path     = "./bin/"
	header   = []string{"TotalTime(ms)", "Flag", "EE1", "EE2", "EE3", "EE4", "EE5", "EE6", "minEE", "Fair index", "Loopcount", "SF1", "TP1", "PRR1", "AveragePRR1", "Received1", "TotalTime1", "TotalPower1", "AverageEE1", "ExisTP1", "IncreGain1", "SF2", "TP2", "PRR2", "AveragePRR2", "Received2", "TotalTime2", "TotalPower2", "AverageEE2", "ExisTP2", "IncreGain2", "SF3", "TP3", "PRR3", "AveragePRR3", "Received3", "TotalTime3", "TotalPower3", "AverageEE3", "ExisTP3", "IncreGain3", "SF4", "TP4", "PRR4", "AveragePRR4", "Received4", "TotalTime4", "TotalPower4", "AverageEE4", "ExisTP4", "IncreGain4", "SF5", "TP5", "PRR5", "AveragePRR5", "Received5", "TotalTime5", "TotalPower5", "AverageEE5", "ExisTP5", "IncreGain5", "SF6", "TP6", "PRR6", "AveragePRR6", "Received6", "TotalTime6", "TotalPower6", "AverageEE6", "ExisTP6", "IncreGain6", "time"}
	row      = 0
)

const (
	FixedHeader = 6 // Header items that are not related to the number of nodes
)

func logData(ED int) {
	if row == 0 {
		fileName = fileName + fileType
		path = path + fileName
	}

	//OpenFile读取文件，不存在时则创建，使用追加模式
	File, err := os.OpenFile(path, os.O_RDWR|os.O_APPEND|os.O_CREATE, 0666)
	if err != nil {
		log.Println("文件打开失败！")
	}
	defer func(File *os.File) {
		err := File.Close()
		if err != nil {

		}
	}(File)

	//创建写入接口
	WriterCsv := csv.NewWriter(File)

	if row == 0 {
		err1 := WriterCsv.Write(header)
		if err1 != nil {
			log.Println("WriterCsv写入文件失败")
		}
	}

	row++

	timeString := strconv.FormatFloat(Totaltime, 'f', 0, 64)

	if Totaltime >= MAXRuntime {
		fmt.Printf("Time exceed! This program will be shut down!\n")
		os.Exit(1)
	}

	str = append(str, timeString)
	EDString := strconv.FormatFloat(float64(ED), 'f', 0, 64)
	str = append(str, EDString)
	for _, m := range EE {
		EEString := strconv.FormatFloat(m, 'f', 6, 64)
		str = append(str, EEString)
	}
	minEEString := strconv.FormatFloat(minEE, 'f', 6, 64)
	str = append(str, minEEString)
	FairnessString := strconv.FormatFloat(Fairness, 'f', 6, 64)
	str = append(str, FairnessString)
	loopcountString := strconv.FormatFloat(loopcount, 'f', 6, 64)
	str = append(str, loopcountString)
	for i := 0; i < M; i++ {
		SFString := strconv.FormatFloat(sfAssigned[i], 'f', 6, 64)
		TPString := strconv.FormatFloat(tpAssigned[i], 'f', 6, 64)
		PRRString := strconv.FormatFloat(PRR[i], 'f', 6, 64)
		AveragePRRSring := strconv.FormatFloat(AveragePRR[i], 'f', 6, 64)
		ReceivedPayloadString := strconv.FormatFloat(ReceivedPayload[i], 'f', 6, 64)
		TotalTransmissionTimeString := strconv.FormatFloat(TotalTransmissionTime[i], 'f', 6, 64)
		TotalTransmissionPowerString := strconv.FormatFloat(TotalTransmissionPower[i], 'f', 6, 64)
		AverageEEString := strconv.FormatFloat(AverageEE[i], 'f', 6, 64)
		ExiTPString := strconv.FormatFloat(tpExisting[i], 'f', 6, 64)
		IncreGainString := strconv.FormatFloat(RealSNRGain[i], 'f', 6, 64)
		str = append(str, SFString)
		str = append(str, TPString)
		str = append(str, PRRString)
		str = append(str, AveragePRRSring)
		str = append(str, ReceivedPayloadString)
		str = append(str, TotalTransmissionTimeString)
		str = append(str, TotalTransmissionPowerString)
		str = append(str, AverageEEString)
		str = append(str, ExiTPString)
		str = append(str, IncreGainString)
	}
	str = append(str, SnapshotTime.Format("2006-01-02T15:04:05Z"))
	//Header length plus time
	if len(str) == (11*M + FixedHeader) {
		//fmt.Println(str)
		err1 := WriterCsv.Write(str)
		if err1 != nil {
			log.Println("WriterCsv写入文件失败")
		}
		WriterCsv.Flush() //刷新，不刷新是无法写入的
		str = str[0:0]
	}
}

func getTotalTime() {
	Totaltime = 1000 * SnapshotTime.Sub(InitTime).Seconds()
	fmt.Printf("INFO: [up] Program total time use in %f ms\n", Totaltime)
}
