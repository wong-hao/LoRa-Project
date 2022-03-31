#pragma once

#ifndef HEADER_2_1_H
#define HEADER_2_1_H

#include "base64.h"
#include "header_1_1.h"
#include "header_1_2.h"
#include "header_1_3.h"
#include "parson.h"

class Buffer {
public:
    int buff_index = 12; /* 12-byte header */

    char *data;//socket接收到的数据
    size_t index;
    int index_backup;      //复制一份index防止deleteChar重复执行多次
    uint8_t uint[BUF_SIZE];//对于Buffer来说是socket接收到的数据的uint形式
    char *inter;           //对于Buffer来说是将bufferi_inter赋值buffer_inter给以后续处理 / 对于BufferSend来说是Upstream JSON data structure
    char *inter_backup;    //对于Buffer来说是复制一份inter防止deleteChar重复执行多次
    uint8_t *inter_uint;
    char *inter_uint_char;              //需要发送的数据的char形式（此时前12-byte header有缺陷，第12 byte后为修改后的Upstream JSON data structure）
    char send_first_part_char[BUF_SIZE];//对于BufferSend来说是12-byte header
    char send_last_part_char[BUF_SIZE]; //对于BufferSend来说是修改后的Upstream JSON data structure
    uint8_t *send;                      //对于BufferSend来说是需要发送的数据 (原始uint8形式)

    uint8_t payload[BUF_SIZE]; /*!> buffer containing the payload *///json里的"data"使用b64_to_bin解码得到的LoRaWAN Frame里的PHY payload
    uint16_t size;                                                  //json数据包里自带的，但mqtt event没有
    char *Hexstring;                                                //对于Buffer来说PHY payload的char形式 / 对于BufferSend来说是char类型的realresult组成的新PHYPayload
    char *Binarystring;                                             //对于BufferSend来说是PHY payload的二进制char形式
    char *Binarystring2;                                            //对于BufferSend来说是Merged error mask / Ambiguity vectors / Va
    char *Binarystring3;                                            //对于BufferSend来说是APC candidate
    char *Binarystring4;                                            //对于BufferSend来说是Soft decoding candidate

    uint8_t *Hexstring_uint8;//对于BufferSend来说是realresult组成的新PHYPayload


    void setData(char *array) const {
        strcpy(data, array);
    }

    void setIndex() {
        index = strlen(data) / 2;
    }

    void setUint() {//将data转换为uint_8类型以进行后续的Json处理
        Char2Uint(data, uint);
    }

    virtual void setInter() {//根据接收到的uint_8类型data获得Upstream JSON data structure
        inter = (char *) (uint + buff_index);
    }

    void setInter_Uint() {//根据Upstream JSON data structure还原出uint8_t类型的data以方便后续处理
        inter_uint = (uint8_t *) (inter - buff_index);
    }


    void setSize(const char *str) {//与net_downlink相似，都是接收到data，故都将str进行base64解码为payload的uint_8形式
        size = b64_to_bin(str, strlen(str), payload, sizeof payload);
    }

    virtual void setHexstring() {//将payload的uint_8形式转换为char形式以方便后续处理
        Uint2Char(payload, Hexstring, size);
    }

    virtual void setBinarystring() {//将payload的十六进制char形式转换为二进制char形式以方便后续处理
        Hex2Bin(Hexstring, Binarystring);
    }
};

class BufferSend : public Buffer {

public:
    int buff_index = 12; /* 12-byte header */

    void setInter(char *array) {
        strcpy(inter, array);
    }

    void setHexstring(char *array) {
        Bin2Hex(array, Hexstring);
    }

    void setBinarystring(char *array1, char *array2) {

        if (OZ_bin_xor(array1, array2, Binarystring) != 0) {
            printf("函数出错！\n");
            return;
        }
    }

    void setMultipleBinarystring(char (*array)[BUF_SIZE], int row) {

        char **array_copy = new char *[row];//避免更改原始数组array，将其复制到copy里

        for (int i = 0; i <= row - 1; i++) {
            array_copy[i] = new char[BUF_SIZE]();
            memset(array_copy[i], 0, BUF_SIZE * sizeof(char));

            for (int j = 0; j < BUF_SIZE; j++) {
                array_copy[i][j] = array[i][j];
            }
        }

        for (int i = 0; i < row - 1; i++) {
            if (OZ_bin_xor(array_copy[i], array_copy[i + 1], array_copy[i + 1]) != 0) {
                printf("函数出错！\n");
                return;
            }
        }

        strcpy(Binarystring, array_copy[row - 1]);

        delete[] array_copy;
    }

    void setSend_First_Part_Char(char *array) {
        strncpy(send_first_part_char, array, buff_index * 2);
        send_first_part_char[strlen(send_first_part_char)] = '\0';
    }

    void setSend_Last_Part_Char() {
        strncpy(send_last_part_char, inter_uint_char + buff_index * 2, strlen(inter_uint_char) - buff_index * 2);
        send_last_part_char[strlen(send_last_part_char)] = '\0';
    }

    void setSend() {
        strcat(send_first_part_char, send_last_part_char);
        Char2Uint(send_first_part_char, send);
    }

    void setInter_Uint_Char(int length) {
        Uint2Char(inter_uint, inter_uint_char, length);
    }

    void setHexstring_uint() {
        Char2Uint(Hexstring, Hexstring_uint8);
    }
};

class Rxpk {
public:
    uint32_t mote_addr;
    uint16_t mote_fcnt;
    int stat;
    int crc_get;
    char *crc;
    uint16_t crc_hex;
    const char *str;//Json里的“data”
    int rssi;
    float snr;
    const char *time;
    char *subtime;//resolution: 10s
    int PayloadSize;

    void setTime(uint8_t array[BUF_SIZE], int length) {
        time = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char *) (array + length))), "rxpk"), 0), "time");
    }

    void setStat(uint8_t array[BUF_SIZE], int length) {
        stat = (int) json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char *) (array + length))), "rxpk"), 0), "stat"));
    }

    void setCrc_get(uint8_t array[BUF_SIZE], int length) {
        crc_get = (int) json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char *) (array + length))), "rxpk"), 0), "crc"));
    }

    void setStr(uint8_t array[BUF_SIZE], int length) {
        str = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char *) (array + length))), "rxpk"), 0), "data");
    }

    void setRssi(uint8_t array[BUF_SIZE], int length) {
        rssi = (int) json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char *) (array + length))), "rxpk"), 0), "rssi"));
    }

    void setSNR(uint8_t array[BUF_SIZE], int length) {
        snr = (float) json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char *) (array + length))), "rxpk"), 0), "lsnr"));
    }

    void setPayloadSize(uint8_t array[BUF_SIZE], int length) {
        PayloadSize = (int) json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char *) (array + length))), "rxpk"), 0), "size"));
    }
};

void Substr(const char *a, int m, int n, char *b);//将a中第m个开始的n个字符复制到b中。https://wenda.so.com/q/1371688591069661

int countED(Buffer *buffer_array, int buffer_num);

int compareTime(Rxpk *rxpk_array, int array_length);

#endif