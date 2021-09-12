#include"header_1_1.h"
#include"header_1_2.h"
#include"header_1_3.h"
#include "payload_crc.h"

#include"payload_diff.h"

#include "base64.h"

#include <iostream>
#include <stdio.h>
#include <cstring>
using namespace std;

char num[BUF_SIZE];
char num2[BUF_SIZE];
int n;

void countone(char* input, int* count){
    for (int i = 0; i < strlen(input); i++) {
        if (input[i] == '1') (*count)++;
    }

}

void insertzero(char* input, int location){

    string input_str(input);
    input_str.insert(location,"0");
    strcpy(input,input_str.c_str());

}

void Search(char* input, int m, char* mch, int crc_int, char* fakeresult, char* realresult, int length, int& pass_crc, int& total_number)
{
    if (pass_crc == 1) {

        return; //flag=1˵���Ѿ���һ��crcУ��ͨ�����ˣ�ֱ���˳���������ֱ�Ӹ�����������false positives (Hash��ײ)

    }

    int i;
    if(m == n)
    {
        //printf("%s",num);
        strcpy(num2,num);

        for(int j=0;j<= strlen(input)-1;j++){
            if(input[j]=='0'){
                insertzero(num2,j);
            }
        }

        //printf("%s\n", num2);

        OZ_bin_xor(mch, num2, fakeresult);
        //printf("%s\n", fakeresult);

        char* Hexstring_temp = new char[BUF_SIZE];
        memset(Hexstring_temp, 0, BUF_SIZE * sizeof(char));

        uint8_t* Hexstring_uint8_temp = new uint8_t[BUF_SIZE];
        memset(Hexstring_uint8_temp, 0, BUF_SIZE * sizeof(uint8_t)); //Ŀǰ��������ؼ��Ķ�̬���飬�����̬�ͻ��ջ����

        uint16_t    payload_crc16_calc_temp = 0;

        Bin2Hex(fakeresult, Hexstring_temp);

        Char2Uint(Hexstring_temp, Hexstring_uint8_temp);
        delete[] Hexstring_temp;

        payload_crc16_calc_temp = sx1302_lora_payload_crc(Hexstring_uint8_temp, length);
        delete[] Hexstring_uint8_temp;

        if (payload_crc16_calc_temp == crc_int){
            strcpy(realresult, fakeresult);
            pass_crc++;
        }

        total_number++;

        printf("Real: %s\n", realresult);
        printf("Total number: %d\n", total_number);
    }
    else
    {
        num[m]='0';Search(input, m+1, mch, crc_int, fakeresult, realresult, length, pass_crc, total_number);
        num[m]='1';Search(input, m+1, mch, crc_int, fakeresult, realresult, length, pass_crc, total_number);
    }
}


void input(char* input, char* mch, int crc_int, char* fakeresult, char* realresult, int length, int& pass_crc, int& total_number) {
    int one = 0;

    countone(input,&one);

    n = one;
    int m = 0;
    Search(input, m, mch, crc_int, fakeresult, realresult, length, pass_crc, total_number);


}



int main()
{

    char x[BUF_SIZE] = "000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000110000";

    char mch[BUF_SIZE] = "010000010000010000010011000001000010011010000000000000010000000000000010110010011010000111101101000011110101010110011011101111111010010100001001100010001101000111100010010001111000110110111000001001100000010010100101111011000111100100001010111111010110010011100111000001000000110111101000100110010001010011000001111000000101010101000100010001001001110110110000110101101110011010010110111111011101000110110000110101111010010100010101001011010111001001001100000110000010100110000011000000001100010001100001";

    char* fakeresult = new char[BUF_SIZE]; //每次candidate与mch异或的中间产值
    memset(fakeresult, 0, BUF_SIZE * sizeof(char));

    char* realresult = new char[BUF_SIZE]; //符合CRC校验的fakeresult
    memset(realresult, 0, BUF_SIZE * sizeof(char));

    char crc_get[BUF_SIZE] = "12364"; //TODO: 从上行数据中获取 (多出来的crc在json中只能使用%u存储)
    unsigned int crc_buffer = atoi(crc_get);
    char crc[BUF_SIZE] = "";
    sprintf(crc, "0x%04X", crc_buffer);
    int crc_int = 0;
    sscanf(crc, "%X", &crc_int); //用sscanf而不是atoi的原因是虽然linux有atoi，但是crc最前面的0还是没了

    int length;
    length = 63;

    int total_number = 0; //一共运行的次数
    int pass_crc = 0; //符合CRC校验的次数

    input(x, mch, crc_int, fakeresult, realresult, length, pass_crc, total_number);


    return 0;
}


