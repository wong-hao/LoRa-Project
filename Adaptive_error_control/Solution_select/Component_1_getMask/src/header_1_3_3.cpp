#include "header_1_1.h"
#include "header_1_3.h"

double getPositiveWeight(char input1, char input2, char input3, char input4) {

    if ((input1 == '1') && (input2 == '1') && (input3 == '1') && (input4 == '1')) {
        return 1.0;
    } else if (((input1 == '1') && (input2 == '1') && (input3 == '1')) || ((input1 == '1') && (input2 == '1') && (input4 == '1')) || ((input2 == '1') && (input3 == '1') && (input4 == '1'))) {
        return 0.75;
    } else if (((input1 == '1') && (input2 == '1')) || ((input1 == '1') && (input3 == '1')) || ((input1 == '1') && (input4 == '1')) || ((input2 == '1') && (input3 == '1')) || ((input2 == '1') && (input4 == '1')) || ((input3 == '1') && (input4 == '1'))) {
        return 0.5;
    } else if (((input1 == '1')) || ((input2 == '1')) || ((input3 == '1')) || ((input4 == '1'))) {
        return 0.25;
    } else {
        return 0;
    }
}

double processData(char input1, char input2, char input3, char input4, float snr1, float snr2, float snr3, float snr4) {
    double input1_copy, input2_copy, input3_copy, input4_copy, positive_weight, negative_weight, snr1_copy, snr2_copy, snr3_copy, snr4_copy, output;

    /* --- STAGE : init weight ---------------------- */

    positive_weight = getPositiveWeight(input1, input2, input3, input4);
    negative_weight = 1 - positive_weight;

    /* --- STAGE : init data ---------------------- */

    if (input1 == '0') {
        input1_copy = -1;
        input1_copy *= negative_weight;
    } else {
        input1_copy = 1;
        input1_copy *= positive_weight;
    }

    if (input2 == '0') {
        input2_copy = -1;
        input2_copy *= negative_weight;
    } else {
        input2_copy = 1;
        input2_copy *= positive_weight;
    }

    if (input3 == '0') {
        input3_copy = -1;
        input1_copy *= negative_weight;
    } else {
        input3_copy = 1;
        input3_copy *= positive_weight;
    }

    if (input4 == '0') {
        input4_copy = -1;
        input4_copy *= negative_weight;
    } else {
        input4_copy = 1;
        input4_copy *= positive_weight;
    }

    /* --- STAGE : init snr ---------------------- */

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

    /* --- STAGE : process data ---------------------- */

    output = input1_copy / snr1_copy + input2_copy / snr2_copy + input3_copy / snr3_copy + input4_copy / snr4_copy;

    return output;
}

void softDecoding(char *input1, char *input2, char *input3, char *input4, char *output, float snr1, float snr2, float snr3, float snr4) {
    if (strlen(input1) != strlen(input2) || strlen(input1) != strlen(input3) || strlen(input1) != strlen(input4) || strlen(input2) != strlen(input3) || strlen(input2) != strlen(input4) || strlen(input3) != strlen(input4)) {
        printf("Length is not equal! Program shut down!\n");
        return;
    }

    size_t size = strlen(input1);

    for (int loopcount = 0; loopcount <= size - 1; loopcount++) {
        if (processData(input1[loopcount], input2[loopcount], input3[loopcount], input4[loopcount], snr1, snr2, snr3, snr4) > 0) {
            output[loopcount] = '1';
        } else {
            output[loopcount] = '0';
        }
    }
    output[size] = '\0';
}
