#include "header_1_1.h"
#include "header_1_3.h"

double processData(char input1, char input2, char input3, char input4, int rssi1, int rssi2, int rssi3, int rssi4) {
    double input1_copy, input2_copy, input3_copy, input4_copy, rssi1_copy, rssi2_copy, rssi3_copy, rssi4_copy;
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

    if (rssi1 < 0) {
        rssi1_copy = -rssi1;
    } else {
        rssi1_copy = rssi1;
    }

    if (rssi2 < 0) {
        rssi2_copy = -rssi2;
    } else {
        rssi2_copy = rssi2;
    }

    if (rssi3 < 0) {
        rssi3_copy = -rssi3;
    } else {
        rssi3_copy = rssi3;
    }

    if (rssi4 < 0) {
        rssi4_copy = -rssi4;
    } else {
        rssi4_copy = rssi4;
    }

    return input1_copy / rssi1_copy + input2_copy / rssi2_copy + input3_copy / rssi3_copy + input4_copy / rssi4_copy;
}

void softDecoding(char *input1, char *input2, char *input3, char *input4, char *output, int rssi1, int rssi2, int rssi3, int rssi4) {
    if (strlen(input1) != strlen(input2) || strlen(input1) != strlen(input3) || strlen(input1) != strlen(input4) || strlen(input2) != strlen(input3) || strlen(input2) != strlen(input4) || strlen(input3) != strlen(input4)) {
        printf("Length is not equal! Program shut down!\n");
        return;
    }

    int size = strlen(input1);


    for (int loopcount = 0; loopcount <= size - 1; loopcount++) {
        if (processData(input1[loopcount], input2[loopcount], input3[loopcount], input4[loopcount], rssi1, rssi2, rssi3, rssi4) > 0) {
            output[loopcount] = '1';
        } else {
            output[loopcount] = '0';
        }
    }
    output[size] = '\0';
}
