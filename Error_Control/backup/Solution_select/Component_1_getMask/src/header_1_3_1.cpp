#include "header_1_3.h"

/* -------------------------------------------------------------------------- */
/* --- STAGE ----Fundamental function---------- */

int getSumofHammingWeight(char input1, char input2, char input3, char input4) {
    return int(input1) / int('1') + int(input2) / int('1') + int(input3) / int('1') + int(input4) / int('1');
}

/* -------------------------------------------------------------------------- */
/* --- STAGE ----PC---------- */


void Hex2Bin(char *source, char *dest) {
    int i = 0;
    char Dict[17][5] =
            {
                    "0000",
                    "0001",
                    "0010",
                    "0011",
                    "0100",
                    "0101",
                    "0110",
                    "0111",
                    "1000",
                    "1001",
                    "1010",
                    "1011",
                    "1100",
                    "1101",
                    "1110",
                    "1111",
            };
    for (i = 0; i < strlen(source); i++) {
        //char temp[5]={0};
        int n = 16;
        if (source[i] >= 'a' && source[i] <= 'f')
            n = source[i] - 'a' + 10;
        if (source[i] >= 'A' && source[i] <= 'F')
            n = source[i] - 'A' + 10;
        if (source[i] >= '0' && source[i] <= '9')
            n = source[i] - '0';
        //sprintf(temp,"%s", Dict[n]);
        //memcpy(&dest[i*4],temp,4);
        memcpy(&dest[i * 4], Dict[n], 4);
    }
    return;
}

int OZ_bin_xor(const char *s1, char *s2, char *dest) {
    unsigned int i;
    int temp1 = 0, temp2 = 0, temp3 = 0;
    if (strlen(s1) != strlen(s2)) {
        printf("错误，不等长！\n");
        return 1;
    }
    for (i = 0; i < strlen(s1); i++) {
        temp1 = s1[i] - '0';
        temp2 = s2[i] - '0';
        temp3 = temp1 ^ temp2;
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

void Bin2Hex(const char *sSrc, char *sDest) {
    size_t times = strlen(sSrc) / 4;
    //char temp[times];
    char *temp = new char[times + 1];//https://blog.csdn.net/weixin_42638401/article/details/88957796
    memset(temp, 0, (times + 1) * sizeof(char));

    int x = 0;
    for (int i = 0; i < times; i++) {
        //int num=8*int(sSrc[i*4])+4*int(sSrc[i*4+1])+2*int(sSrc[i*4+2])+int(sSrc[i*4+3]);
        x = 8 * (sSrc[i * 4] - '0');
        x += 4 * (sSrc[i * 4 + 1] - '0');
        x += 2 * (sSrc[i * 4 + 2] - '0');
        x += sSrc[i * 4 + 3] - '0';
        sprintf(temp + i, "%1x", x);
    }
    memcpy(sDest, temp, times);

    delete[] temp;
}

/* -------------------------------------------------------------------------- */
/* --- Fourth XOR ---------------------- */

int OZ_forth_bin_xor(const char *s1, char *s2, char *s3, char *s4, char *dest) {
    unsigned int i;
    int temp1 = 0, temp2 = 0, temp3 = 0, temp4 = 0, temp5 = 0;
    if ((strlen(s1) != strlen(s2)) || (strlen(s1) != strlen(s3))) {
        printf("错误，不等长！\n");
        return 1;
    }
    for (i = 0; i < strlen(s1); i++) {
        temp1 = s1[i] - '0';
        temp2 = s2[i] - '0';
        temp3 = s3[i] - '0';
        temp4 = s4[i] - '0';
        temp5 = temp1 ^ temp2 ^ temp3 ^ temp4;
        if (temp5 == 1)
            dest[i] = '1';
        else if (temp5 == 0)
            dest[i] = '0';
        else {
            printf("字符串内容有误！\n");
            return 1;
        }
    }
    return 0;
}
