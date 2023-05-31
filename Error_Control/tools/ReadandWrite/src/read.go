package src

import (
	"encoding/csv"
	"fmt"
	"io"
	"io/ioutil"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"time"
)

var (
	filename string

	col1Data []string
	col2Data []string
	col3Data []string
	col4Data []string
	col5Data []string
	lastRow  int

	stat1  int
	stat2  int
	stat3  int
	stat4  int
	result int
)

// ReadColumnFromCSV 读取指定csv文件指定列的数据
func ReadColumnFromCSV(filename string, col1 int, col2 int, col3 int, col4 int, col5 int) ([]string, []string, []string, []string, []string, error) {
	// 打开csv文件
	f, err := ioutil.ReadFile(filename)
	if err != nil {
		return nil, nil, nil, nil, nil, err
	}

	// 从每行中读取指定的列数据
	r := csv.NewReader(strings.NewReader(string(f)))
	for {
		record, err := r.Read()
		if err == io.EOF {
			break
		}
		if err != nil {
			return nil, nil, nil, nil, nil, err
		}
		col1Data = append(col1Data, record[col1])
		col2Data = append(col2Data, record[col2])
		col3Data = append(col3Data, record[col3])
		col4Data = append(col4Data, record[col4])
		col5Data = append(col5Data, record[col5])
	}

	// 返回结果
	return col1Data, col2Data, col3Data, col4Data, col5Data, nil
}

// ReadColumnFromCSVInDir 读取本级子目录bin下最后一个csv文件的指定列数据
func ReadColumnFromCSVInDir(col1 int, col2 int, col3 int, col4 int, col5 int) ([]string, []string, []string, []string, []string, error) {
	// 获取当前目录路径
	cwd, err := os.Getwd()
	if err != nil {
		return nil, nil, nil, nil, nil, err
	}

	fmt.Printf("Searching directory:%s\n", cwd)

	// 获取本级子目录bin下的所有文件
	files, err := os.ReadDir(".")
	if err != nil {
		return nil, nil, nil, nil, nil, err
	}

	// 统计CSV文件数量
	csvCount := 0
	for _, file := range files {
		if filepath.Ext(file.Name()) == ".csv" {
			csvCount++
		}
	}

	if csvCount == 0 {
		return nil, nil, nil, nil, nil, fmt.Errorf("没有找到CSV文件")
	}

	// 找到最后一个CSV文件
	lastCSVIndex := csvCount - 1
	for _, file := range files {
		if filepath.Ext(file.Name()) == ".csv" {
			if lastCSVIndex == 0 {
				// 读取最后一个CSV文件的指定列数据
				filename = file.Name()
				return ReadColumnFromCSV(file.Name(), col1, col2, col3, col4, col5)
			}
			lastCSVIndex--
		}
	}

	// 读取指定csv文件的指定列数据
	if filename != "" {
		return ReadColumnFromCSV(filename, col1, col2, col3, col4, col5)
	}

	return nil, nil, nil, nil, nil, fmt.Errorf("没有找到最后一个csv文件")
}

// ReadLastLineFromCSVInDir 每隔interval秒读取本级子目录bin下的第fileNum个csv文件的最后一行数据，并将其中的第col列与第4*col2数据存储到变量里
func ReadLastLineFromCSVInDir(col int, col2 int, col3 int, col4 int, col5, interval int) {
	for {
		col1Data, col2Data, col3Data, col4Data, col5Data, _ = ReadColumnFromCSVInDir(col, col2, col3, col4, col5)

		if len(col1Data) > 0 && len(col2Data) > 0 && len(col3Data) > 0 && len(col4Data) > 0 && len(col5Data) > 0 {
			lastRow = len(col1Data) - 1
			stat1, _ = strconv.Atoi(col1Data[lastRow])
			stat2, _ = strconv.Atoi(col2Data[lastRow])
			stat3, _ = strconv.Atoi(col3Data[lastRow])
			stat4, _ = strconv.Atoi(col4Data[lastRow])
			result, _ = strconv.Atoi(col5Data[lastRow])
			fmt.Printf("Current File Name: %s. stat1：%d, stat2: %d, stat3: %d, stat4: %d, result: %d\n", filename, stat1, stat2, stat3, stat4, result) //Get current time
			if stat1 != 0 && stat2 != 0 && stat3 != 0 && stat4 != 0 {
				SnapshotTime = time.Now()
				influxdbWriteOptimization(SnapshotTime)
			}
		} else {
			fmt.Println("文件中没有数据")
		}

		time.Sleep(time.Duration(interval) * time.Second)
	}
}
