#pragma once

#include "header_1_1.h"
#include "header_1_2.h"
#include "header_1_3.h"
#include "base64.h"
#include "parson.h"

class Buffer
{
public:
    int buff_index = 12; /* 12-byte header */

    char* data;
    int index;
    uint8_t uint[BUF_SIZE];
    char* inter;
    uint8_t* inter_uint;
    char* inter_uint_char;
    char send_first_part_char[BUF_SIZE];
    char send_last_part_char[BUF_SIZE];
    uint8_t* send;

    uint8_t  payload[BUF_SIZE];   /*!> buffer containing the payload */
    uint16_t size; //json数据包里自带的，但mqtt event没有
    char* Hexstring;
    char* Binarystring;
    char* Binarystring2;
    char* Binarystring3;

    uint8_t* Hexstring_uint8;

    void setData(char* array){
        strcpy(data, array);
    }

    void setIndex(){
        index = strlen(data)/2;
    }

    void setUint(){
        Char2Uint(data, uint);

    }

    virtual void setInter(){
        inter = (char*)(uint + buff_index);
    }

    void setInter_Uint(){
        inter_uint = (uint8_t*)(inter - buff_index);
    }


    void setSize(const char* str){
        size = b64_to_bin(str, strlen(str), payload, sizeof payload);
    }

    virtual void setHexstring(){
        Uint2Char(payload, Hexstring, size);
    }

    virtual void setBinarystring(){
        Hex2Bin(Hexstring, Binarystring);
    }


};

class BufferSend : public Buffer{

public:
    int buff_index = 12; /* 12-byte header */

    void setInter(char* array){
        strcpy(inter, array);

    }

    void setHexstring(char* array){
        Bin2Hex(array, Hexstring);
    }

    void setBinarystring(char* array1, char* array2){

        if (OZ_bin_xor(array1, array2, Binarystring) != 0)
        {
            printf("函数出错！\n");
            return;
        }
    }

    void setForthBinarystring(char* array1, char* array2, char* array3, char* array4){

        if (OZ_forth_bin_xor(array1, array2, array3, array4, Binarystring) != 0)
        {
            printf("函数出错！\n");
            return;
        }
    }

    void setSend_First_Part_Char(char* array){
        strncpy(send_first_part_char, array, buff_index*2);
        send_first_part_char[strlen(send_first_part_char)] = '\0';
    }

    void setSend_Last_Part_Char(){
        strncpy(send_last_part_char, inter_uint_char + buff_index*2, strlen(inter_uint_char) - buff_index*2);
        send_last_part_char[strlen(send_last_part_char)] = '\0';
    }

    void setSend(){
        strcat(send_first_part_char, send_last_part_char);
        Char2Uint(send_first_part_char, send);
    }

    void setInter_Uint_Char(int num){
        Uint2Char(inter_uint, inter_uint_char, num);
    }

    void setHexstring_uint(){
        Char2Uint(Hexstring, Hexstring_uint8);
    }
};

class Rxpk
{
public:
    int stat;
    int crc_get;
    const char* str;
    int rssi;
    const char* time;

    void setTime(uint8_t array[BUF_SIZE], int num){
        time = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(array + num))), "rxpk"), 0), "time");
    }

    void setStat(uint8_t array[BUF_SIZE], int num){
        stat = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(array + num))), "rxpk"), 0), "stat"));
    }

    void setCrc_get(uint8_t array[BUF_SIZE], int num){
        crc_get = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(array + num))), "rxpk"), 0), "crc"));
    }

    void setStr(uint8_t array[BUF_SIZE], int num){
        str = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(array+num))), "rxpk"), 0), "data");
    }

    void setRssi(uint8_t array[BUF_SIZE], int num){
        rssi = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(array+num))), "rxpk"), 0), "rssi"));
    }
};

int countED(Buffer* buffer_array, int buffer_num);

int compareTime(Rxpk* rxpk_array, int buffer_num);