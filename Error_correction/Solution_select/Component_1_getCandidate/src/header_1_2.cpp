#include"header_1_1.h"
#include"header_1_2.h"
#include"header_1_3.h"
#include "payload_crc.h"

#include"payload_diff.h"

#include "base64.h"

char num[BUF_SIZE];
char num2[BUF_SIZE];
int n;

void insertzero(char* input, int location){

    std::string input_str(input);
    input_str.insert(location,"0");
    strcpy(input,input_str.c_str());

}

void Search(char* input, int m, char* mch, int crc_int, char* fakeresult, char* realresult, int length, int& pass_crc, int& total_number)
{
    if (pass_crc == 1) {

        return; //pass_crc=1说明已经有一个crc校验通过的了，直接退出，这样会直接根除掉假阳性false positives (Hash碰撞)

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

        /*测试代码
        total_number++;
        printf("The number of candidate: %d\n", total_number);
        */

        //TODO: hidden errors
        //TODO: 超时退出程序：//https://blog.csdn.net/codedz/article/details/80387001, 其实可以不用，最大纠错比特位数量Hamming_weight_max已经可以用来限制了

    }
    else
    {
        num[m]='0';Search(input, m+1, mch, crc_int, fakeresult, realresult, length, pass_crc, total_number);
        num[m]='1';Search(input, m+1, mch, crc_int, fakeresult, realresult, length, pass_crc, total_number);
    }
}


void correct(char* input, char* mch, int Hamming_weight_now, int crc_int, char* fakeresult, char* realresult, int length, int& pass_crc, int& total_number) {

    n = Hamming_weight_now;
    int m = 0;
    Search(input, m, mch, crc_int, fakeresult, realresult, length, pass_crc, total_number);

}


