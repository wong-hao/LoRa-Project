//https://blog.csdn.net/weixin_30279751/article/details/95437814

#include <stdio.h>
#include <string.h> //十六进制字符串转二进制字符串
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> /* C99 types */
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <stdio.h>
#include <algorithm>

void Hex2Bin(char* source, char* dest, int len)
{
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
    for (i = 0; i < len; i++)
    {
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

int OZ_bin_xor(const char* s1, char* s2, char* dest)
{
    unsigned int i;
    int temp1 = 0, temp2 = 0, temp3 = 0;
    if (strlen(s1) != strlen(s2))
    {
        printf("错误，不等长！\n");
        return 1;
    }
    for (i = 0; i < strlen(s1); i++)
    {
        temp1 = s1[i] - '0';
        temp2 = s2[i] - '0';
        temp3 = temp1 ^ temp2;
        if (temp3 == 1)
            dest[i] = '1';
        else if (temp3 == 0)
            dest[i] = '0';
        else
        {
            printf("字符串内容有误！\n");
            return 1;
        }
    }
    return 0;
}

void Bin2Hex(const char* sSrc, char* sDest, int nSrcLen)
{
    int times = nSrcLen / 4;
    //char temp[times];
    char* temp = new char[times + 1]; //https://blog.csdn.net/weixin_42638401/article/details/88957796

    int x = 0;
    for (int i = 0; i < times; i++)
    {
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

char s[256], d[256]; //s是Merged error mask；d是Error candidate pattern
void outmystr(int n, char* input, char* compare, char* interoutput, char* finaloutput)
{
    OZ_bin_xor(input, d, interoutput);

    if (strcmp(interoutput, compare)==0) {
        strcpy(finaloutput, interoutput); 
        //这里是把最后一个符合条件的赋值给realoutput（sPM应该是发现符合条件之后马上结束程序的，但是return不管用就先算了）

    }

    if (n < 0) {


        printf("Candidate: %s ", d);
        printf("Interoutput: %s\n", interoutput);

    }
    else
    {
        d[n] = '0';
        outmystr(n - 1, input, compare, interoutput, finaloutput);
        if (s[n] == '1')
        {
            d[n] = '1';
            outmystr(n - 1, input, compare, interoutput, finaloutput);
        }

    }

}


int main()
{
    uint8_t Hexstring1[64] = "9a\0"; //m's
    uint8_t Binarystring1[256] = { 0 };
    uint8_t Hexstring2[64] = "56\0"; //m'r
    uint8_t Binarystring2[256] = { 0 };
    uint8_t Hexstring3[64] = { 0 };
    uint8_t Binarystring3[256] = { 0 }; //Merged error mask / Ambiguity vectors / Va

    /* -------------------------------------------------------------------------- */
    /* --- STAGE 1: uint8_t转char ---------------------- */ //https://bbs.csdn.net/topics/390141308

    char* Hexstring11 = (char*)Hexstring1;
    char* Binarystring11 = (char*)Binarystring1;
    char* Hexstring21 = (char*)Hexstring2;
    char* Binarystring21 = (char*)Binarystring2;
    char* Hexstring31 = (char*)Hexstring3;
    char* Binarystring31 = (char*)Binarystring3;

    printf("M's: %s\n", Hexstring1);
    printf("M'r: %s\n", Hexstring2);
    /* -------------------------------------------------------------------------- */
    /* --- STAGE 2: 十六进制字符串转二进制字符串 ---------------------- */
    Hex2Bin(Hexstring11, Binarystring11, strlen(Hexstring11));
    Hex2Bin(Hexstring21, Binarystring21, strlen(Hexstring21));

    /* -------------------------------------------------------------------------- */
    /* --- STAGE 3: 二进制字符串异或 ---------------------- */
    if (OZ_bin_xor(Binarystring11, Binarystring21, Binarystring31) != 0)
    {
        printf("函数出错！\n");
        return 1;
    }


    char mch[256] = "";
    strcpy(mch, Binarystring11);
    printf("MCH: %s\n", mch);

    char crc[256] = "00010010";
    printf("CRC: %s\n", crc);

    int i = 0;
    strcpy(s, Binarystring31);
    printf("Mask: %s\n", s);

    char fakeresult[256] = "";
    char realresult[256] = "";




    while (s[i])
        d[i++] = '0';

    outmystr(i - 1,mch, crc, fakeresult, realresult);

    printf("Realresult: %s\n", realresult);





    /* -------------------------------------------------------------------------- */
    /* --- STAGE 4: 二进制字符串转十六进制字符串 ---------------------- */

    Bin2Hex(Binarystring31, Hexstring31, strlen(Binarystring31));
    //puts(Hexstring31);

    return 0;
}