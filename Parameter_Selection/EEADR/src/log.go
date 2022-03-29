package src

import (
	"encoding/csv"
	"log"
	"os"
	"strconv"
	"time"
)

var (
	StartTime = time.Now() // 获取当前时间

	str      []string
	fileName = StartTime.Format("2006-01-02-15-04-05")
	fileType = "-Dataset.csv"
	path     = "./bin/"
	header   = []string{"TotalTime(ms)", "EE1", "EE2", "Fair index", "time"}
	row      = 0
)

func logData(totaltime float64, EE [M]float64, Fairness float64) {
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

	timeString := strconv.FormatFloat(totaltime, 'f', 0, 64)
	str = append(str, timeString)
	for _, m := range EE {
		EEString := strconv.FormatFloat(m, 'f', 6, 64)
		str = append(str, EEString)
	}
	FairnessString := strconv.FormatFloat(Fairness, 'f', 0, 64)
	str = append(str, FairnessString)
	str = append(str, EndTime.Format("2006-01-02T15:04:05Z"))

	if len(str) == (M + 3) {
		//fmt.Println(str)
		err1 := WriterCsv.Write(str)
		if err1 != nil {
			log.Println("WriterCsv写入文件失败")
		}
		WriterCsv.Flush() //刷新，不刷新是无法写入的
		str = str[0:0]
	}
}
