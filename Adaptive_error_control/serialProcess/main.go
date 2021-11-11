package main

import (
	"bufio"
	"encoding/csv"
	"fmt"
	"log"
	"os"
	"strings"
	"time"
)
import "github.com/jacobsa/go-serial/serial"


const(
	INT_MAX = int(^uint32((0)) >> 1)
)

var(
	str []string
	fileName string
	fileType string = "-PHYPayload.csv"
	path string = "E:/"
	header = [] string { "PHYPayload" , "CRC"}
)

func check(err error) {
	if err != nil {
		panic(err.Error())
	}
}

func main() {

	fileName := time.Now().Format("2006-01-02-15-04-05")
	fileName = fileName + fileType
	path = path + fileName

	options := serial.OpenOptions{
		PortName:        "COM6",
		BaudRate:        115200,
		DataBits:        8,
		StopBits:        2,
		MinimumReadSize: 4,
	}
	stream, err := serial.Open(options) //https://stackoverflow.com/questions/50088669/golang-reading-from-serial
	if err != nil {
		fmt.Printf("could not open port '%s': FileNotFoundError(2, '系统找不到指定的文件。', None, 2)\n",options.PortName)
		log.Fatal(err)
	}
	//OpenFile读取文件，不存在时则创建，使用追加模式
	File,err:=os.OpenFile(path,os.O_RDWR|os.O_APPEND|os.O_CREATE,0666)
	if err!=nil{
		log.Println("文件打开失败！")
	}
	defer func(File *os.File) {
		err := File.Close()
		if err != nil {

		}
	}(File)

	//创建写入接口
	WriterCsv:=csv.NewWriter(File)

	for i := 0; i < INT_MAX; i++ {
		scanner := bufio.NewScanner(stream)

		if i==0 {
			err1:=WriterCsv.Write(header)
			if err1!=nil{
				log.Println("WriterCsv写入文件失败")
			}
		}

		for scanner.Scan() {
			if strings.Contains(scanner.Text(), "PHY Payload") || strings.Contains(scanner.Text(), "CRC") {
				str = append(str, scanner.Text())
			}

			if len(str) == 2{
				fmt.Println(str)
				err1:=WriterCsv.Write(str)
				if err1!=nil{
					log.Println("WriterCsv写入文件失败")
				}
				WriterCsv.Flush() //刷新，不刷新是无法写入的
				str = str[0:0]
			}

			fmt.Println(scanner.Text()) // Println will add back the final '\n'
		}

		if err := scanner.Err(); err != nil {
			log.Fatal(err)
		}
	}




}