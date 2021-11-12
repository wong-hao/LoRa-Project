#include "header_1_1.h"
#include "header_1_2.h"
#include "header_1_3.h"
#include "header_2_2.h"
#include "payload_crc.h"

#include "payload_diff.h"

#include "base64.h"

int main() {

    initFile();
    openFile();

    uint8_t payload1[BUF_SIZE]; /*!> buffer containing the payload *///Json里面"data"部分 / 使用b64_to_bin解码得到LoRaWAN Frame里的PHY payload

    char str1[BUF_SIZE] = "QQQTBCaAAQACyaHtD1Wbv6UJiNHiR424JgSl7HkK/WTnBA3omRTB4FVERJ2w1uaW/dGw16UVLXJMGCmDAMRh";
    uint16_t size1;                                                   //json数据包里自带的，但mqtt event没有
    size1 = b64_to_bin(str1, strlen(str1), payload1, sizeof payload1);//与net_downlink相似，都是接收到data，故都用b64_to_bin
    printf("Copy_1 of data: %s\n", str1);


    uint8_t payload2[BUF_SIZE]; /*!> buffer containing the payload */
    char str2[BUF_SIZE] = "QAQTBCaAAQACyaHtD1Wbv6UJiNHiR424JgSl7HkK/WTnBA3omRTB4FVERJ2w1uaW/dGw16UVLXJMGCmDAMRR";
    uint16_t size2;                                                   //json数据包里自带的，但mqtt event没有
    size2 = b64_to_bin(str2, strlen(str2), payload2, sizeof payload2);//与net_downlink相似，都是接收到data，故都用b64_to_bin
    printf("Copy_2 of data: %s\n", str2);

    uint8_t payload3[BUF_SIZE]; /*!> buffer containing the payload */
    char str3[BUF_SIZE] = "QAQTBCaAAQACyaHtD1Wbv6UJiNHiR425JgSl7HkK/WTnBA3omRTB4FVERJ2w1uaW/dGw16UVLXJMGCmDAMRR";
    uint16_t size3;                                                   //json数据包里自带的，但mqtt event没有
    size3 = b64_to_bin(str3, strlen(str3), payload3, sizeof payload3);//与net_downlink相似，都是接收到data，故都用b64_to_bin
    printf("Copy_3 of data: %s\n", str2);

    uint8_t payload4[BUF_SIZE]; /*!> buffer containing the payload */
    char str4[BUF_SIZE] = "QAQTBCaAAQACyaHtD1Wbv6UJiNHiR424JgSl7HkK/WTnBA3omRTB4FVERJ3w1uaW/dGw16UVLXJMGCmDAMRR";
    uint16_t size4;                                                   //json数据包里自带的，但mqtt event没有
    size4 = b64_to_bin(str4, strlen(str4), payload4, sizeof payload4);//与net_downlink相似，都是接收到data，故都用b64_to_bin
    printf("Copy_4 of data: %s\n", str4);

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : Decoding ---------------------- */

    uint16_t size;

    if ((size1 == size2) && (size1 == size3) && (size1 == size4)) {
        size = size1;
    } else {
        printf("Error: Not all copies has the same length! This program will be shut down!\n");
        return 0;
    }

    int Hamming_weight_now = 0;
    getFourthNe(payload1, payload2, payload3, payload4, size, Hamming_weight_now);//Calculate Hamming weight

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : uint8_t转char ---------------------- *///https://bbs.csdn.net/topics/390141308


    char Hexstring1[BUF_SIZE] = {0};
    Uint2Char(payload1, Hexstring1, size);

    char Hexstring2[BUF_SIZE] = {0};
    Uint2Char(payload2, Hexstring2, size);

    char Hexstring3[BUF_SIZE] = {0};
    Uint2Char(payload3, Hexstring3, size);

    char Hexstring4[BUF_SIZE] = {0};
    Uint2Char(payload4, Hexstring4, size);

    printf("PHY Payload1 get: %s\n", Hexstring1);
    printf("PHY Payload2 get: %s\n", Hexstring2);
    printf("PHY Payload3 get: %s\n", Hexstring3);
    printf("PHY Payload4 get: %s\n", Hexstring4);

    logPHYPayload(Hexstring1);
    logPHYPayload(Hexstring2);
    logPHYPayload(Hexstring3);
    logPHYPayload(Hexstring4);

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 十六进制字符串转二进制字符串 ---------------------- *///https://blog.csdn.net/weixin_30279751/article/details/95437814

    char Binarystring1[BUF_SIZE] = {0};
    char Binarystring2[BUF_SIZE] = {0};
    char Binarystring3[BUF_SIZE] = {0};
    char Binarystring4[BUF_SIZE] = {0};

    Hex2Bin(Hexstring1, Binarystring1);
    Hex2Bin(Hexstring2, Binarystring2);
    Hex2Bin(Hexstring3, Binarystring3);
    Hex2Bin(Hexstring4, Binarystring4);

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : GetCandidate ---------------------- */
    /* -------------------------------------------------------------------------- */
    /* --- STAGE : CRC ---------------------- */


    char mch[BUF_SIZE] = {0};
    strcpy(mch, Binarystring1);
    /* 测试代码
     printf("MCH: %s\n", mch);
     */
    char crc_get[BUF_SIZE] = "12364";
    unsigned int crc_buffer = atoi(crc_get);
    char crc[BUF_SIZE] = "";
    sprintf(crc, "0x%04X", crc_buffer);
    printf("Processed CRC: %s\n", crc);
    int crc_int = 0;
    sscanf(crc, "%X", &crc_int);//用sscanf而不是atoi的原因是虽然linux有atoi，但是crc最前面的0还是没了

    logCRC(crc);
    logCRC(crc);
    logCRC(crc);
    logCRC(crc);

    logLine();

    /* 测试代码
    printf("CRC int: %x\n", crc_int);
    printf("Mask: %s\n", s);
    */

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 纠错 ---------------------- */

    char fakeresult[BUF_SIZE] = {0};//每次candidate与mch异或的中间产值
    memset(fakeresult, 0, BUF_SIZE * sizeof(char));

    char realresult[BUF_SIZE] = {0};//符合CRC校验的fakeresult，但不保证能通过MIC校验
    memset(realresult, 0, BUF_SIZE * sizeof(char));

    int total_number = 0;//一共运行的次数
    int pass_crc = 0;    //符合CRC校验的次数

    struct timespec startTime;
    clock_gettime(CLOCK_REALTIME, &startTime);

    switch (StageOption) {
        case 0: {
            if (strlen(realresult) == 0) {

                /* -------------------------------------------------------------------------- */
                /* --- STAGE : EPC ---------------------- */

                printf("%s\n", "EPC start!");


                char Binarystring5[BUF_SIZE] = {0};//Merged error mask / Ambiguity vectors

                OZ_forth_bin_xor(Binarystring1, Binarystring2, Binarystring3, Binarystring4, Binarystring5);

                if (Hamming_weight_now > Hamming_weight_max) {

                    printf("%s: %d\n", "Hamming weight is larger than the max number", Hamming_weight_max);
                    printf("This program will be shut down!\n");
                    return 0;
                }

                /*测试代码
                printf("Hamming_weight_now: %d\n", Hamming_weight_now);
                */

                memset(realresult, 0, BUF_SIZE * sizeof(char));
                memset(fakeresult, 0, BUF_SIZE * sizeof(char));
                total_number = 0;//一共运行的次数
                pass_crc = 0;    //符合CRC校验的次数

                if (Hamming_weight_now <= Hamming_weight_max / 2) {
                    incremental_correct(Binarystring5, mch, Hamming_weight_now, crc_int, fakeresult, realresult, size, pass_crc, total_number, startTime);
                } else {
                    correct(Binarystring5, mch, Hamming_weight_now, crc_int, fakeresult, realresult, size, pass_crc, total_number, startTime);
                }

                if (strlen(realresult) == 0) {
                    printf("%s\n", "Error can not be fixed with EPC!");
                }
            }
        }
        case 1: {
            if (strlen(realresult) == 0) {
                printf("%s\n", "APC start!");
                //CRC未出错的话一定出现了hidden error

                /* -------------------------------------------------------------------------- */
                /* --- STAGE : APC ---------------------- */


                char Binarystring8[BUF_SIZE] = {0};//Merged error mask / Ambiguity vectors / Va
                char Binarystring9[BUF_SIZE] = {0};//APC candidate

                Hamming_weight_now = 0;

                LeastReliableMask(Binarystring1, Binarystring2, Binarystring3, Binarystring4, Binarystring8, Hamming_weight_now);// Calculate Hamming weight
                majorityVoting(Binarystring1, Binarystring2, Binarystring3, Binarystring4, Binarystring9);

                /* -------------------------------------------------------------------------- */
                /* --- STAGE : GetCandidate ---------------------- */
                /* -------------------------------------------------------------------------- */
                /* --- STAGE : CRC ---------------------- */

                /* 测试代码
                printf("MCH: %s\n", mch);
                */

                /* 测试代码
                printf("CRC int: %x\n", crc_int);
                printf("Mask: %s\n", s);
                */

                if (Hamming_weight_now > Hamming_weight_max) {

                    printf("%s: %d\n", "Hamming weight is larger than the max number", Hamming_weight_max);
                    printf("This program will be shut down!\n");
                    return 0;
                }

                /*测试代码
                printf("Hamming_weight_now: %d\n", Hamming_weight_now);
                */

                memset(fakeresult, 0, BUF_SIZE * sizeof(char));
                memset(realresult, 0, BUF_SIZE * sizeof(char));
                total_number = 0;//一共运行的次数
                pass_crc = 0;    //符合CRC校验的次数

                validateCRC(crc_int, Binarystring9, realresult, size, pass_crc);
                if (strlen(realresult) == 0) {
                    printf("%s\n", "Error can not be fixed! APC continues!");

                    if (Hamming_weight_now <= Hamming_weight_max / 2) {
                        incremental_correct(Binarystring8, mch, Hamming_weight_now, crc_int, fakeresult, realresult, size, pass_crc, total_number, startTime);
                    } else {
                        correct(Binarystring8, mch, Hamming_weight_now, crc_int, fakeresult, realresult, size, pass_crc, total_number, startTime);
                    }

                    if (strlen(realresult) == 0) {
                        printf("%s\n", "Error can not be fixed with APC!");
                    }
                }
            }
        }
        case 2: {
            if (strlen(realresult) == 0) {
                printf("%s\n", "Soft decoding begins!");
                //CRC未出错的话一定出现了hidden error

                /* -------------------------------------------------------------------------- */
                /* --- STAGE : Soft decoding ---------------------- */

                char Binarystring9[BUF_SIZE] = {0};//Soft decoding candidate

                softDecoding(Binarystring1, Binarystring2, Binarystring3, Binarystring4, Binarystring9, -50, -60, -70, -80);
                validateCRC(crc_int, Binarystring9, realresult, size, pass_crc);

                if (strlen(realresult) == 0) {
                    printf("%s\n", "Error can not be fixed with soft decision!");
                }
            }
            break;
        }
        default: {
            printf("StageOption is illegal! This program will be shut down!\n");
            return 0;
        }
    }

    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &endTime);

    struct timespec interv;
    diff(&startTime, &endTime, &interv);

    struct timespec endTime2;
    clock_gettime(CLOCK_REALTIME, &endTime2);

    struct timespec interv2;
    diff(&startTime, &endTime2, &interv2);
    cout << "Total timeuse: " << double(interv2.tv_sec * NANOSECOND + interv2.tv_nsec) / NANOSECOND << "s" << endl;

    /* 测试代码
    printf("RealresultBit: %s\n", realresult);
    */

    /* 测试代码 需更改if(flag == 1)判断条件为flag==2及以上数字，否则永远不会出现假阳性
    if (pass_crc > 1){
        printf("%s\n", "Falsepositive happens");
    }
    */


    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 二进制字符串转十六进制字符串 ---------------------- */


    char Hexstring5[BUF_SIZE] = {0};//char类型的PHYPayload

    Bin2Hex(realresult, Hexstring5);
    /* 测试代码
    printf("RealresultHex: %s\n", Hexstring5);
    */

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : Encoding ---------------------- */

    uint8_t Hexstring5_uint8[BUF_SIZE] = {0};

    Char2Uint(Hexstring5, Hexstring5_uint8);
    printf("Corrected PHY Payload%d: ");
    for (int loopcount = 0; loopcount < size; loopcount++) {
        printf("%02X", Hexstring5_uint8[loopcount]);
    }
    printf("\n");

    uint8_t data_up_uint8[BUF_SIZE] = {0};//不用太大， 因为原代码里的buff_up不止装的data所以很大
    bin_to_b64(Hexstring5_uint8, size, (char *) (data_up_uint8), BUF_SIZE);

    char data_up[BUF_SIZE] = {0};
    strcpy(data_up, (char *) (data_up_uint8));
    printf("Corrected data: %s\n", data_up);

    /* 测试代码
    uint16_t    payload_crc16_calc;
    payload_crc16_calc = sx1302_lora_payload_crc(Hexstring4_uint8, size);
    printf("FixedPayload CRC (0x%04X)\n", payload_crc16_calc);
    */

    return 0;
}