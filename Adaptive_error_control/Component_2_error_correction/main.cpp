#include"header_1_1.h"
#include"header_1_2.h"
#include"header_1_3.h"
#include "payload_crc.h"

#include"payload_diff.h"

#include "base64.h"

int main()
{
    /* -------------------------------------------------------------------------- */
    /* --- STAGE : Decoding ---------------------- */


    uint8_t  payload1[BUF_SIZE];   /*!> buffer containing the payload */
    char str1[BUF_SIZE] = "QQQTBCaAAQACyaHtD1Wbv6UJiNHiR424JgSl7HkK/WTnBA3omRTB4FVERJ2w1uaW/dGw16UVLXJMGCmDAMRh";
    uint16_t size1; //json数据包里自带的，但mqtt event没有
    size1 = b64_to_bin(str1, strlen(str1), payload1, sizeof payload1); //与net_downlink相似，都是接收到data，故都用b64_to_bin
    printf("Copy_1 of data: %s\n", str1);


    uint8_t  payload2[BUF_SIZE];   /*!> buffer containing the payload */
    char str2[BUF_SIZE] = "QAQTBCaAAQACyaHtD1Wbv6UJiNHiR424JgSl7HkK/WTnBA3omRTB4FVERJ2w1uaW/dGw16UVLXJMGCmDAMRR";
    uint16_t size2; //json数据包里自带的，但mqtt event没有
    size2 = b64_to_bin(str2, strlen(str2), payload2, sizeof payload2); //与net_downlink相似，都是接收到data，故都用b64_to_bin
    printf("Copy_1 of data: %s\n", str2);


    uint16_t size;

    if (size1 == size2) {
        size = size1;
    }
    else {
        printf("Error: length1 is not equal to length2. This program will be shut down!");
        return 0;
    }

    int Hamming_weight_now = 0;
    getNe(payload1, payload2, size, Hamming_weight_now);

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : uint8_t转char ---------------------- */ //https://bbs.csdn.net/topics/390141308


    char Hexstring1[BUF_SIZE] = { 0 };
    Uint2Char(payload1, Hexstring1, size);
    /*测试代码
    printf("M's: %s\n", Hexstring1);
    */

    char Hexstring2[BUF_SIZE] = { 0 };
    Uint2Char(payload2, Hexstring2, size);
    /*测试代码
	printf("M'r: %s\n", Hexstring2);
	*/

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 十六进制字符串转二进制字符串 ---------------------- */ //https://blog.csdn.net/weixin_30279751/article/details/95437814

    char Binarystring1[BUF_SIZE] = { 0 };
    char Binarystring2[BUF_SIZE] = { 0 };

    Hex2Bin(Hexstring1, Binarystring1);
    Hex2Bin(Hexstring2, Binarystring2);


    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 二进制字符串异或 ---------------------- */


    char Binarystring3[BUF_SIZE] = { 0 }; //Merged error mask / Ambiguity vectors / Va

    if (OZ_bin_xor(Binarystring1, Binarystring2, Binarystring3) != 0) //TODO: Majority voting / more than two copies
    {
        printf("函数出错！\n");
        return 1;
    }

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : GetCandidate ---------------------- */
    /* -------------------------------------------------------------------------- */
    /* --- STAGE : CRC ---------------------- */


    char mch[BUF_SIZE] = { 0 };
    strcpy(mch, Binarystring1);  //TOOD: 根据rssis比较获得mch
    /* 测试代码
     printf("MCH: %s\n", mch);
     */
    char crc_get[BUF_SIZE] = "12364"; //TODO: 从上行数据中获取 (多出来的crc在json中只能使用%u存储)
    unsigned int crc_buffer = atoi(crc_get);
    char crc[BUF_SIZE] = "";
    sprintf(crc, "0x%04X", crc_buffer);
    printf("Processed CRC: %s\n", crc);
    int crc_int = 0;
    sscanf(crc, "%X", &crc_int); //用sscanf而不是atoi的原因是虽然linux有atoi，但是crc最前面的0还是没了
    /* 测试代码
    printf("CRC int: %x\n", crc_int);
    */
    int i = 0;
    /* 测试代码
    printf("Mask: %s\n", s);
    */

    int Hamming_weight_max = 30; //预设的最多纠错比特位数量
    if (Hamming_weight_now > Hamming_weight_max) {

        printf("%s: %d\n", "Hamming weight is larger than the max number" ,Hamming_weight_max);
        printf("This program will be shut down!\n");
        return 0;

    } {
        printf("Hamming Weight: %d\n", Hamming_weight_now);
    }

    char fakeresult[BUF_SIZE] = { 0 }; //每次candidate与mch异或的中间产值
    char realresult[BUF_SIZE] = { 0 }; //符合CRC校验的fakeresult
    int total_number = 0; //一共运行的次数
    int pass_crc = 0; //符合CRC校验的次数

    struct timespec startTime;
    clock_gettime(CLOCK_REALTIME, &startTime);

    if(Hamming_weight_now <= Hamming_weight_max/2){
        incremental_correct(Binarystring3, mch, Hamming_weight_now, crc_int, fakeresult, realresult, size, pass_crc, total_number, startTime);
    }else{
        correct(Binarystring3, mch, Hamming_weight_now, crc_int, fakeresult, realresult, size, pass_crc, total_number, startTime);
    }

    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &endTime);

    struct timespec interv;
    diff(&startTime, &endTime, &interv);
    cout<<"Total timeuse: "<<double(interv.tv_sec * NANOSECOND + interv.tv_nsec)/NANOSECOND<<"s"<<endl;

    if (strlen(realresult)==0) {
        printf("%s\n", "Error can not be fixed! This program will be shut down!");
        //CRC未出错的话一定出现了hidden error
        return 0;
    }

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


    char Hexstring4[BUF_SIZE] = { 0 }; //char类型的PHYPayload

    Bin2Hex(realresult, Hexstring4);
    /* 测试代码
    printf("RealresultHex: %s\n", Hexstring4);
    */

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : Encoding ---------------------- */

    uint8_t  Hexstring4_uint8[BUF_SIZE] = { 0 };

    Char2Uint(Hexstring4, Hexstring4_uint8);

    uint8_t data_up_uint8[BUF_SIZE] = { 0 }; //不用太大， 因为原代码里的buff_up不止装的data所以很大
    bin_to_b64(Hexstring4_uint8, size, (char*)(data_up_uint8), BUF_SIZE);

    char data_up[BUF_SIZE] = { 0 };
    strcpy(data_up, (char*)(data_up_uint8));
    printf("Corrected data: %s\n", data_up);

    /* 测试代码
    uint16_t    payload_crc16_calc;
    payload_crc16_calc = sx1302_lora_payload_crc(Hexstring4_uint8, size);
    printf("FixedPayload CRC (0x%04X)\n", payload_crc16_calc);
    */

    return 0;
}