#include "header_2_1.h"


int countED(Buffer *buffer_array, int buffer_num) {
    int nodenum = 0;

    for (int i = 0; i <= buffer_num - 1; i++) {
        if (buffer_array[i].index) nodenum++;
    }
    return nodenum;
}

int compareTime(Rxpk *rxpk_array, int buffer_num) {
    int flag = 1;

    for (int i = 0; i <= buffer_num - 1; i++) {
        if (rxpk_array[i].time != rxpk_array[0].time) {
            flag = 0;
        }
    }

    return flag;
}