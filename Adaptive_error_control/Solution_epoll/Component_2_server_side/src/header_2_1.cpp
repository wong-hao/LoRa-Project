#include "header_2_1.h"


int countED(Buffer *buffer_array, int array_length) {
    int nodenum = 0;

    for (int i = 0; i <= array_length - 1; i++) {
        if (buffer_array[i].index) nodenum++;
    }
    return nodenum;
}

int compareTime(Rxpk *rxpk_array, int array_length) {
    int flag = 1;

    for (int i = 0; i <= array_length - 1; i++) {
        if (strcmp(rxpk_array[i].time, rxpk_array[0].time) != 0) {
            flag = 0;
        }
    }

    return flag;
}