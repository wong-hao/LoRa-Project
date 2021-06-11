#include "header_1_2.h"
void outmystr(int n, char* input, char* compare, char* interoutput, char* finaloutput)
{

    if (strcmp(interoutput, compare) == 0) {
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

    char mch[BUF_SIZE] = "1010";
    char fakeresult[BUF_SIZE] = { 0 };
    char realresult[BUF_SIZE] = { 0 };
    char crc[BUF_SIZE] = "1010";


    int i = 0;
    char Binarystring31[] = "0100000000000100000100110000010000100110100000000000000000000000000000100011001001000101000111010010010100111100000000000010100011011010010111001010111111010111011110110101010001001001000110111010100110010101101111011100011000101001010010010000000010001001100111110100011000011100000010000101110010001000";
    strcpy(s, Binarystring31);
    printf("Mask: %s\n", s);

    ;
    /*

    int one = 0; //非零位个数：用于生成二维字符数组
    for (int j = 0; j < strlen(s); j++) {
        if (s[j] == '1') {
            one++;
        }
    }

    char* result = new char[strlen(s) + 1];//用于copy到一维字符数组
    memset(result, 0, strlen(s) + 1);
    */


    while (s[i])
        d[i++] = '0';

    outmystr(i - 1,mch, crc, fakeresult, realresult);




    return 0;
}
