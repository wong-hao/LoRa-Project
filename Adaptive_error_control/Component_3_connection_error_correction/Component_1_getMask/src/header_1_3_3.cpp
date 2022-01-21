#include "header_1_1.h"
#include "header_1_3.h"

double getPositiveWeight(char input1, char input2, char input3, char input4, int input_size) {

    return getSumofHammingWeight(input1, input2, input3, input4) / double(input_size);

}

double processData(char input1, char input2, char input3, char input4, float* SNRArray, int input_size) {
    double input1_copy, input2_copy, input3_copy, input4_copy, positive_weight, negative_weight, snr1_copy, snr2_copy, snr3_copy, snr4_copy, output;

    /* --- STAGE : init weight ---------------------- */

    positive_weight = getPositiveWeight(input1, input2, input3, input4, input_size);
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

    /* --- STAGE : process data ---------------------- */

    output = input1_copy * snr1_copy + input2_copy * snr2_copy + input3_copy * snr3_copy + input4_copy * snr4_copy;

    return output;
}

void softDecoding(char (*input)[BUF_SIZE], int input_size, char *output, float* SNRArray) {
    int flag = 1;

    for (int i = 0; i <= input_size - 1; i++) {
        if (sizeof(input[i]) != sizeof(input[0])) {
            flag = 0;
        }
    }

    if (flag == 0) {
        printf("Length is not equal! Program shut down!\n");
        return;
    }

    size_t size = strlen(input[0]);

    for (int loopcount = 0; loopcount <= size - 1; loopcount++) {
        if (processData(input[0][loopcount], input[1][loopcount], input[2][loopcount], input[3][loopcount], SNRArray, input_size) >= 0) {
            output[loopcount] = '1';
        } else {
            output[loopcount] = '0';
        }
    }
    output[size] = '\0';
}
