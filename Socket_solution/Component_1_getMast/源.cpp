#include "header_1_3.h"

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
    memset(temp, 0, (times + 1) * sizeof(char));

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

int main()
{
    uint8_t Hexstring1[BUF_SIZE] = "80041304268000000232451D253CC028DA5CAFD77B54491BA995BDC6294900899F461C085C88\0"; //m's
    uint8_t Binarystring1[BUF_SIZE] = { 0 };
    uint8_t Hexstring2[BUF_SIZE] = "40041304268000000232451D253C0028DA5CAFD77B54491BA995BDC6294900899F461C085C88\0"; //m'r
    uint8_t Binarystring2[BUF_SIZE] = { 0 };
    uint8_t Hexstring3[BUF_SIZE] = { 0 };
    uint8_t Binarystring3[BUF_SIZE] = { 0 }; //Merged error mask / Ambiguity vectors / Va

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : uint8_t转char ---------------------- */ //https://bbs.csdn.net/topics/390141308

    char* Hexstring11 = (char*)Hexstring1;
    char* Binarystring11 = (char*)Binarystring1;
    char* Hexstring21 = (char*)Hexstring2;
    char* Binarystring21 = (char*)Binarystring2;
    char* Hexstring31 = (char*)Hexstring3;
    char* Binarystring31 = (char*)Binarystring3;

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 十六进制字符串转二进制字符串 ---------------------- */
    Hex2Bin(Hexstring11, Binarystring11, strlen(Hexstring11));
    Hex2Bin(Hexstring21, Binarystring21, strlen(Hexstring21));

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 二进制字符串异或 ---------------------- */
    if (OZ_bin_xor(Binarystring11, Binarystring21, Binarystring31) != 0)
    {
        printf("函数出错！\n");
        return 1;
    }
    puts(Binarystring31);

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 二进制字符串转十六进制字符串 ---------------------- */

    Bin2Hex(Binarystring31, Hexstring31, strlen(Binarystring31));
    //puts(Hexstring31);

    return 0;
}