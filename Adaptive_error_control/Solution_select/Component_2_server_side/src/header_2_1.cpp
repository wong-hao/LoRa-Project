#include "header_2_1.h"

void Substr(const char *a, int m, int n, char *b) {
    int i, j = 0;
    for (i = m - 1; i < m + n - 1; i++)
        b[j++] = a[i];
    b[j] = '\0';
}

int countED(Buffer *buffer_array, int buffer_num) {
    int nodenum = 0;

    for (int i = 0; i <= buffer_num - 1; i++) {
        if (buffer_array[i].index) nodenum++;
    }
    return nodenum;
}

int compareTime(Rxpk *rxpk_array, int array_length) {
    int flag = 1;

    for (int i = 0; i <= array_length - 1; i++) {
        if (strlen(rxpk_array[i].time)) {

            rxpk_array[i].subtime = new char[BUF_SIZE];
            memset(rxpk_array[i].subtime, 0, BUF_SIZE * sizeof(char));

            Substr(rxpk_array[i].time, 1, 18, rxpk_array[i].subtime);

            if (strcmp(rxpk_array[i].subtime, rxpk_array[0].subtime) != 0) {
                flag = 0;
            }
        }
    }

    return flag;
}