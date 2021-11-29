#pragma once

#include "base64.h"
#include "header_1_1.h"
#include "header_1_2.h"
#include "header_1_3.h"
#include "parson.h"

class Buffer {
public:
    int buff_index = 12; /* 12-byte header */

    char *data;//socket接收到的数据
    int index;
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

    char *DevAddr;
    char *crc;
    int crc_int;


    void setData(char *array) {
        strcpy(data, array);
    }

    void setIndex() {
        index = strlen(data) / 2;
    }

    void setUint() {
        Char2Uint(data, uint);
    }

    virtual void setInter() {
        inter = (char *) (uint + buff_index);
    }

    void setInter_Uint() {
        inter_uint = (uint8_t *) (inter - buff_index);
    }


    void setSize(const char *str) {
        size = b64_to_bin(str, strlen(str), payload, sizeof payload);
    }

    virtual void setHexstring() {
        Uint2Char(payload, Hexstring, size);
    }

    virtual void setBinarystring() {
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

    void setForthBinarystring(char *array1, char *array2, char *array3, char *array4) {

        if (OZ_forth_bin_xor(array1, array2, array3, array4, Binarystring) != 0) {
            printf("函数出错！\n");
            return;
        }
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

    void setInter_Uint_Char(int num) {
        Uint2Char(inter_uint, inter_uint_char, num);
    }

    void setHexstring_uint() {
        Char2Uint(Hexstring, Hexstring_uint8);
    }
};

class Rxpk {
public:
    int DevAddr_get;
    int stat;
    int crc_get;
    const char *str;//Json里的“data”
    int rssi;
    float snr;
    const char *time;
    int fcnt;
    int PayloadSize;

    void setDevAddr_get(uint8_t array[BUF_SIZE], int num) {
        DevAddr_get = (int) json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char *) (array + num))), "rxpk"), 0), "DevAddr"));
    }

    void setTime(uint8_t array[BUF_SIZE], int num) {
        time = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char *) (array + num))), "rxpk"), 0), "time");
    }

    void setFcnt(uint8_t array[BUF_SIZE], int num) {
        fcnt = (int) json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char *) (array + num))), "rxpk"), 0), "fcnt"));
    }

    void setStat(uint8_t array[BUF_SIZE], int num) {
        stat = (int) json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char *) (array + num))), "rxpk"), 0), "stat"));
    }

    void setCrc_get(uint8_t array[BUF_SIZE], int num) {
        crc_get = (int) json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char *) (array + num))), "rxpk"), 0), "crc"));
    }

    void setStr(uint8_t array[BUF_SIZE], int num) {
        str = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char *) (array + num))), "rxpk"), 0), "data");
    }

    void setRssi(uint8_t array[BUF_SIZE], int num) {
        rssi = (int) json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char *) (array + num))), "rxpk"), 0), "rssi"));
    }

    void setSNR(uint8_t array[BUF_SIZE], int num) {
        snr = (float) json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char *) (array + num))), "rxpk"), 0), "lsnr"));
    }

    void setPayloadSize(uint8_t array[BUF_SIZE], int num) {
        PayloadSize = (int) json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char *) (array + num))), "rxpk"), 0), "size"));
    }
};

int countED(Buffer *buffer_array, int buffer_num);

int compareTime(Rxpk *rxpk_array, int array_length);//防止fcnt与CRC一样出现错误导致无法一起处理

int compareFcnt(Rxpk *rxpk_array, int array_length);//防止网关接收时间相差一秒导致无法一起进行处理