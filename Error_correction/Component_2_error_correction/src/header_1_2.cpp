#include "header_1_1.h"
#include "header_1_2.h"
#include "header_1_3.h"

#include "payload_crc.h"

char s[BUF_SIZE], d[BUF_SIZE]; //s是Merged error mask；d是Error candidate pattern

void outmystr(int n, char* input, int compare, char* interoutput, char* finaloutput, int length, int& flag, int& test) //https://bbs.csdn.net/topics/399153127
{
    if (flag == 1) {

        return; //flag=1说明已经有一个crc校验通过的了，直接退出，这样会直接根除掉假阳性false positives (Hash碰撞)

    }

    OZ_bin_xor(input, d, interoutput);

    char Hexstring_temp[BUF_SIZE] = { 0 }; //char类型的PHYPayload
    uint8_t  Hexstring_uint8_temp[BUF_SIZE] = { 0 };
    uint16_t    payload_crc16_calc_temp = 0;

    Bin2Hex(interoutput, Hexstring_temp);

    Char2Uint(Hexstring_temp, Hexstring_uint8_temp);

    payload_crc16_calc_temp = sx1302_lora_payload_crc(Hexstring_uint8_temp, length);

    if (n < 0) {

        if (payload_crc16_calc_temp == compare) {
            strcpy(finaloutput, interoutput);

            flag++;

            /* 测试代码
            printf("Pass crc check time: %d and it happends at candidate number: %d\n", flag, test);
            printf("The Passed InterPHYPayload : ");
            for (int count = 0; count < length; count++) {
                printf("%02X", Hexstring_uint8_temp[count]);
            }
            printf("\n");

            char Hexstring4[BUF_SIZE] = { 0 }; //char类型的PHYPayload
            Bin2Hex(interoutput, Hexstring4);
            uint8_t  Hexstring4_uint8[BUF_SIZE] = { 0 };
            Char2Uint(Hexstring4, Hexstring4_uint8, length);
            uint8_t data_up_uint8[BUF_SIZE] = { 0 }; //不用太大， 因为原代码里的buff_up不止装的data所以很大
            bin_to_b64(Hexstring4_uint8, length, (char*)(data_up_uint8), BUF_SIZE);
            char data_up[BUF_SIZE] = { 0 };
            strcpy(data_up, (char*)(data_up_uint8));
            printf("The Passed Interresult: %s\n", data_up);

            printf("\n");
            printf("The passed Payload CRC (0x%04X)\n", payload_crc16_calc_temp);
            */

            //TODO: hidden errors
            //TODO: 超时退出程序：//https://blog.csdn.net/codedz/article/details/80387001, 其实可以不用，最大纠错比特位数量Hamming_weight_max已经可以用来限制了
        }

        /* 测试代码
        printf("Candidate: %s\n", d);
        printf("Interoutput: %s\n", interoutput);
        */

        /* 测试代码
        test++;

        printf("The number of candidate: %d\n", test);
        printf("Every InterPHYPayload: \n");
        for (int count = 0; count < length; count++) {
            printf("%02X", Hexstring_uint8_temp[count]);
        }
        printf("Every Payload CRC (0x%04X)\n", payload_crc16_calc_temp);
        printf("\n");
        */
    }
    else
    {
        d[n] = '0';
        outmystr(n - 1, input, compare, interoutput, finaloutput, length, flag, test);
        if (s[n] == '1')
        {
            d[n] = '1';
            outmystr(n - 1, input, compare, interoutput, finaloutput, length, flag, test);
        }

    }

}