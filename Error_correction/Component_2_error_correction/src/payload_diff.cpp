#include "payload_diff.h"

void OZ_hex_xor(uint8_t* array1, uint8_t* array2, uint8_t* array3, int size)
{
    for (int j = 0; j < size; j++)
    {
        array3[j] = array1[j] ^ array2[j];
    }
}

void getNe(uint8_t* array1, uint8_t* array2, int size, int& number) {

    int i, j;
    uint8_t payload_diff[255];

    /* Count how many bits differs */ //https://github.com/Lora-net/sx1302_hal/blob/master/tools/payload_tools/payload_diff.c#L71

    OZ_hex_xor(array1, array2, payload_diff, size);

    for (j = 0; j < size; j++) {
        for (i = 7; i >= 0; i--) {
            if (TAKE_N_BITS_FROM(payload_diff[j], i, 1) == 1) {
                number += 1;
            }
        }
    }
}

/*
void getNe(char* array, int& number) {

    for (unsigned int i = 0; i < strlen(array); i++) {
        if (array[i] == '1') {
            number++;
        }
    }
}
*/