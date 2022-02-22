#include "header_1_1.h"
#include "header_1_3.h"

void processData(char (*input)[BUF_SIZE], int size, float *SNRArray, int row, char *output) {

    double sum, sum2;
    double positive_weight, negative_weight;
    double *input_copy = new double[row];

    for (int i = 0; i <= size - 1; i++) {

        sum = 0;
        sum2 = 0;
        memset(input_copy, 0, row * sizeof(double));

        /* --- STAGE : init weight ---------------------- */

        for (int loopcount = 0; loopcount <= row - 1; loopcount++) {
            sum += int(input[loopcount][i]) / int('1');
        }

        positive_weight = sum / double(row);
        negative_weight = 1 - positive_weight;

        if (positive_weight == 1.00) {
            output[i] = '1';
            output[size] = '\0';
            continue;
        } else if (positive_weight == 0.00) {
            output[i] = '0';
            output[size] = '\0';
            continue;
        }

        for (int loopcount = 0; loopcount <= row - 1; loopcount++) {

            /* --- STAGE : init data ---------------------- */

            if (input[loopcount][i] == '0') {
                input_copy[loopcount] = -1;
                input_copy[loopcount] *= negative_weight;
            } else {
                input_copy[loopcount] = 1;
                input_copy[loopcount] *= positive_weight;
            }

            /* --- STAGE : process data ---------------------- */

            sum2 += input_copy[loopcount] * SNRArray[loopcount];
        }

        if (sum2 >= 0) {
            output[i] = '1';
            output[size] = '\0';
            continue;

        } else {
            output[i] = '0';
            output[size] = '\0';
            continue;
        }
    }
    return;
}

void softDecoding(char (*input)[BUF_SIZE], int input_size, char *output, float *SNRArray) {
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

    processData(input, size, SNRArray, input_size, output);

    return;
}
