#include "header_1_1.h"
#include "header_1_3.h"

int OZ_bin_and(const char *s1, char *s2, char *dest) {
    unsigned int i;
    int temp1 = 0, temp2 = 0, temp3 = 0;
    if (strlen(s1) != strlen(s2)) {
        printf("错误，不等长！\n");
        return 1;
    }
    for (i = 0; i < strlen(s1); i++) {
        temp1 = s1[i] - '0';
        temp2 = s2[i] - '0';
        temp3 = temp1 & temp2;
        if (temp3 == 1)
            dest[i] = '1';
        else if (temp3 == 0)
            dest[i] = '0';
        else {
            printf("字符串内容有误！\n");
            return 1;
        }
    }
    return 0;
}

void getOutput1(char (*input)[BUF_SIZE], int row, int size, char *output, int &Hammming_weight_now) {
    for (int i = 0; i <= size - 1; i++) {
        int sum = 0;

        for (int loopcount = 0; loopcount <= row - 1; loopcount++) {
            sum += int(input[loopcount][i]) / int('1');
        }
        if (sum == 0 || sum == row) {
            output[i] = '0';
        } else {
            output[i] = '1';
            Hammming_weight_now++;
        }
    }

    return;
}

void getOutput2(char (*input)[BUF_SIZE], int row, int size, char *output) {
    for (int i = 0; i <= size - 1; i++) {
        int sum = 0;

        for (int loopcount = 0; loopcount <= row - 1; loopcount++) {
            sum += int(input[loopcount][i]) / int('1');
        }
        if (sum >= row / 2) {
            output[i] = '1';
        } else {
            output[i] = '0';
        }
    }

    return;
}

void majorityVoting(char (*input)[BUF_SIZE], int input_size, char *output) {
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

    getOutput2(input, input_size, size, output);

    return;
}

void LeastReliableMask(char (*input)[BUF_SIZE], int input_size, char *output, int &Hammming_weight_now) {

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

    getOutput1(input, input_size, size, output, Hammming_weight_now);

    return;
}
