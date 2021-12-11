#include "header_1_1.h"
#include "header_1_3.h"

double processData(char input1, char input2, char input3, char input4, float snr1, float snr2, float snr3, float snr4) {
    double input1_copy, input2_copy, input3_copy, input4_copy, snr1_copy, snr2_copy, snr3_copy, snr4_copy;
    if (input1 == '0') {
        input1_copy = -1;
    } else {
        input1_copy = 1;
    }

    if (input2 == '0') {
        input2_copy = -1;
    } else {
        input2_copy = 1;
    }

    if (input3 == '0') {
        input3_copy = -1;
    } else {
        input3_copy = 1;
    }

    if (input4 == '0') {
        input4_copy = -1;
    } else {
        input4_copy = 1;
    }

    if (snr1 < 0) {
        snr1_copy = -snr1;
    } else {
        snr1_copy = snr1;
    }

    if (snr2 < 0) {
        snr2_copy = -snr2;
    } else {
        snr2_copy = snr2;
    }

    if (snr3 < 0) {
        snr3_copy = -snr3;
    } else {
        snr3_copy = snr3;
    }

    if (snr4 < 0) {
        snr4_copy = -snr4;
    } else {
        snr4_copy = snr4;
    }

    return input1_copy / snr1_copy + input2_copy / snr2_copy + input3_copy / snr3_copy + input4_copy / snr4_copy;
}

void softDecoding(char *input1, char *input2, char *input3, char *input4, char *output, float snr1, float snr2, float snr3, float snr4) {
    if (strlen(input1) != strlen(input2) || strlen(input1) != strlen(input3) || strlen(input1) != strlen(input4) || strlen(input2) != strlen(input3) || strlen(input2) != strlen(input4) || strlen(input3) != strlen(input4)) {
        printf("Length is not equal! Program shut down!\n");
        return;
    }

    int size = strlen(input1);


    for (int loopcount = 0; loopcount <= size - 1; loopcount++) {
        if (processData(input1[loopcount], input2[loopcount], input3[loopcount], input4[loopcount], snr1, snr2, snr3, snr4) > 0) {
            output[loopcount] = '1';
        } else {
            output[loopcount] = '0';
        }
    }
    output[size] = '\0';
}
