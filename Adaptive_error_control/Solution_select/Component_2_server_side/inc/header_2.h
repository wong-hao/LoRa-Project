#pragma once
#include "parson.h"

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