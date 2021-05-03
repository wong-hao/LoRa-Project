#include "header_1_1.h"

void Char2Uint(char* array, uint8_t* array_uint) {

    /* Convert hex string to byte array */ //https://github.com/Lora-net/sx1302_hal/blob/master/tools/payload_tools/payload_crc.c#L39
    int payload_size = strlen(array) / 2;
    for (int j = 0; j < payload_size; j++) {
        sscanf(array + 2 * j, "%02hhx", &array_uint[j]);
    }
}

/*
void Char2Uint(char* array, uint8_t* array_uint, int length) {

    for (int count = 0; count < 2 * length; count++) {
        if (count % 2 == 0) {
            char buff_char[256] = { 0 };
            strncpy(buff_char, array + count, 2); //https://blog.csdn.net/zmhawk/article/details/44600075
            buff_char[strlen(buff_char)] = '\0';
            sscanf(buff_char, "%X", (int*)(&array_uint[count / 2])); //https://bbs.csdn.net/topics/391935459
        }
    }
}
*/

void Uint2Char(uint8_t* array_uint, char* array, int length) {


    for (uint16_t count = 0; count < length; count++) {
        char buff[256] = { 0 };
        sprintf(buff, "%02X", array_uint[count]);
        strcat(array, buff);

    }

}
