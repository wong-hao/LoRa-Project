#include "../../Component_1_fakesend/inc/header_1_1.h"
#include "../inc/header_1_2.h"
#include "../../Component_1_getMask/inc/header_1_3.h"

#include "../../tools/inc/payload_crc.h"

char s[BUF_SIZE], d[BUF_SIZE]; //s��Merged error mask��d��Error candidate pattern

void outmystr(int n, char* input, int compare, char* interoutput, char* finaloutput, int length, int& flag, int& test) //https://bbs.csdn.net/topics/399153127
{
    if (flag == 1) {

        return; //flag=1˵���Ѿ���һ��crcУ��ͨ�����ˣ�ֱ���˳���������ֱ�Ӹ�����������false positives (Hash��ײ)

    }

    OZ_bin_xor(input, d, interoutput);

    char* Hexstring_temp = new char[BUF_SIZE];
    memset(Hexstring_temp, 0, BUF_SIZE * sizeof(char));

    uint8_t* Hexstring_uint8_temp = new uint8_t[BUF_SIZE];
    memset(Hexstring_uint8_temp, 0, BUF_SIZE * sizeof(uint8_t)); //Ŀǰ��������ؼ��Ķ�̬���飬�����̬�ͻ��ջ����

    uint16_t    payload_crc16_calc_temp = 0;

    Bin2Hex(interoutput, Hexstring_temp);

    Char2Uint(Hexstring_temp, Hexstring_uint8_temp);
    delete[] Hexstring_temp;

    payload_crc16_calc_temp = sx1302_lora_payload_crc(Hexstring_uint8_temp, length);
    delete[] Hexstring_uint8_temp;

    if (n < 0) {

        if (payload_crc16_calc_temp == compare) {
            strcpy(finaloutput, interoutput);

            flag++;

            /* ���Դ���

            printf("Pass crc check time: %d and it happends at candidate number: %d\n", flag, test);
            printf("The Passed InterPHYPayload : ");
            for (int count = 0; count < length; count++) {
                printf("%02X", Hexstring_uint8_temp[count]);
            }
            printf("\n");

            char* Hexstring4 = new char[BUF_SIZE]; //char���͵�PHYPayload
            memset(Hexstring4, 0, BUF_SIZE * sizeof(char));
            Bin2Hex(interoutput, Hexstring4);
            uint8_t* Hexstring4_uint8 = new uint8_t[BUF_SIZE];
            memset(Hexstring4_uint8, 0, BUF_SIZE * sizeof(uint8_t));
            Char2Uint(Hexstring4, Hexstring4_uint8);
            uint8_t* data_up_uint8 = new uint8_t[BUF_SIZE]; //����̫�� ��Ϊԭ�������buff_up��ֹװ��data���Ժܴ�
            memset(data_up_uint8, 0, BUF_SIZE * sizeof(uint8_t));
            bin_to_b64(Hexstring4_uint8, length, (char*)(data_up_uint8), BUF_SIZE);
            char* data_up = new char[BUF_SIZE]; //char���͵�PHYPayload
            memset(data_up, 0, BUF_SIZE * sizeof(char));
            strcpy(data_up, (char*)(data_up_uint8));
            printf("The Passed Interresult: %s\n", data_up);

            printf("\n");
            printf("The passed Payload CRC (0x%04X)\n", payload_crc16_calc_temp);
            */

            //TODO: hidden errors
            //TODO: ��ʱ�˳�����//https://blog.csdn.net/codedz/article/details/80387001, ��ʵ���Բ��ã����������λ����Hamming_weight_max�Ѿ���������������
        }

        /* ���Դ���
        printf("Candidate: %s\n", d);
        printf("Interoutput: %s\n", interoutput);
        */

        /* ���Դ���
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