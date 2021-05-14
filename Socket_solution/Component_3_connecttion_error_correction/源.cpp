#include "header_1_1.h"
#include "header_1_2.h"
#include "header_1_3.h"
#include "payload_crc.h"
#include "payload_diff.h"
#include "header_2_2.h"

#include "header_3.h"

#include "base64.h"
#include "parson.h"
#include "cJSON.h"



extern char s[BUF_SIZE], d[BUF_SIZE]; //s是Merged error mask；d是Error candidate pattern


int main()

{
    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 模拟server_side接收到的buffer数据进行试验---------------------- */
    //TODO: 取多个真实值实验
    char buffer1[BUF_SIZE] = "02AA1A000016C001FF10D3F67B227278706B223A5B7B226A766572223A312C22746D7374223A3830323838333330392C2274696D65223A22323032312D30342D31335430323A34383A30372E3030303030303030305A222C22746D6D73223A313330323331373238373030302C226368616E223A302C2272666368223A302C2266726571223A3438362E3330303030302C226D6964223A31302C2273746174223A2D312C22637263223A33333735302C226D6F6475223A224C4F5241222C2264617472223A225346374257313235222C22636F6472223A22342F35222C227273736973223A2D37372C226C736E72223A2D362E352C22666F6666223A2D3235372C2272737369223A2D37312C2273697A65223A31382C2264617461223A225969515442435741435441435A455878472B614E4A7A4E6B227D2C7B226A766572223A312C22746D7374223A3830323838333331312C2274696D65223A22323032312D30342D31335430323A34383A30372E3030303030303030305A222C22746D6D73223A313330323331373238373030302C226368616E223A352C2272666368223A312C2266726571223A3438372E3330303030302C226D6964223A20382C2273746174223A312C22637263223A33333735302C226D6F6475223A224C4F5241222C2264617472223A225346374257313235222C22636F6472223A22342F35222C227273736973223A2D31302C226C736E72223A31332E322C22666F6666223A2D3236322C2272737369223A2D392C2273697A65223A31382C2264617461223A2251415154424361414351414356306278472B614E4A7A4E6B227D5D7D";
    char buffer2[BUF_SIZE] = "02AA1A000016C001FF10D3F77B227278706B223A5B7B226A766572223A312C22746D7374223A3830303933323837322C2274696D65223A22323032312D30342D31335430323A34383A30372E3030303030303030305A222C22746D6D73223A313330323331373238373030302C226368616E223A302C2272666368223A302C2266726571223A3438362E3330303030302C226D6964223A31312C2273746174223A2D312C22637263223A33333735302C226D6F6475223A224C4F5241222C2264617472223A225346374257313235222C22636F6472223A22342F35222C227273736973223A2D39312C226C736E72223A2D382E352C22666F6666223A2D3238372C2272737369223A2D38332C2273697A65223A31382C2264617461223A2251415154424F627342564148416B5078472B614E4A7A4E6B227D2C7B226A766572223A312C22746D7374223A3830303933323937372C2274696D65223A22323032312D30342D31335430323A34383A30372E3030303030303030305A222C22746D6D73223A313330323331373238373030302C226368616E223A352C2272666368223A312C2266726571223A3438372E3330303030302C226D6964223A20382C2273746174223A312C22637263223A33333735302C226D6F6475223A224C4F5241222C2264617472223A225346374257313235222C22636F6472223A22342F35222C227273736973223A2D33312C226C736E72223A31332E382C22666F6666223A2D3236392C2272737369223A2D33312C2273697A65223A31382C2264617461223A2251415154424361414351414356306278472B614E4A7A4E6B227D5D7D";

    int buff_index1 = strlen(buffer1) / 2;
    int buff_index2 = strlen(buffer2) / 2;

    uint8_t* buffer_uint1 = new uint8_t[BUF_SIZE];
    memset(buffer_uint1, 0, BUF_SIZE * sizeof(uint8_t));

    uint8_t* buffer_uint2 = new uint8_t[BUF_SIZE];
    memset(buffer_uint2, 0, BUF_SIZE * sizeof(uint8_t));

    Char2Uint(buffer1, buffer_uint1);
    Char2Uint(buffer2, buffer_uint2);

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 对中间数据buffer_inter纠错---------------------- */

    char* buffer1_inter = (char*)(buffer_uint1 + 12); //json字符串的char值
    char* buffer2_inter = (char*)(buffer_uint2 + 12);

    printf("buffer1_inter: %s\n", buffer1_inter);
    printf("\n");
    printf("buffer2_inter: %s\n", buffer2_inter);
    printf("\n");

    uint8_t* buffer1_inter_uint = (uint8_t*)(buffer1_inter - 12);
    uint8_t* buffer2_inter_uint = (uint8_t*)(buffer2_inter - 12);

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 找到上行数据中需要的属性的值 ---------------------- */
    //TODO: 解决多数据包同时上行情况 (重复数据包接收是因为距离太近)
    //https://forum.rakwireless.com/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/3980/3?u=haowong
    //https://forum.chirpstack.io/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/10886/2?u=shirou_emiya

    char report1[BUF_SIZE] = "stat";
    char report2[BUF_SIZE] = "data";

    const char* time1 = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint1 + 12))), "rxpk"), 0), "time");
    const char* time2 = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint2 + 12))), "rxpk"), 0), "time");

    int stat1 = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint1 + 12))), "rxpk"), 0), "stat"));
    int stat2 = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint2 + 12))), "rxpk"), 0), "stat"));


    int crc_get1 = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint1 + 12))), "rxpk"), 0), "crc"));
    int crc_get2 = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint2 + 12))), "rxpk"), 0), "crc"));
    unsigned int crc_get = 0;

    const char* str1 = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint1 + 12))), "rxpk"), 0), "data");
    const char* str2 = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint2 + 12))), "rxpk"), 0), "data");

    int rssi1 = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint1 + 12))), "rxpk"), 0), "rssi"));
    int rssi2 = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint2 + 12))), "rxpk"), 0), "rssi"));


    /*测试代码
    printf("stat1: %d\n", stat1);
    printf("crc_get1: %d\n", crc_get1);
    printf("str1: %s\n", str1);
    printf("rssis1: %d\n", rssis1);
    printf("time1: %s\n", time1);
    */


    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 当两个上行数据都错且crc值相同时进行纠错 ---------------------- */


    if ((stat1== -1) && (stat2 == -1)) {


        printf("Both two packets are crc incorrect\n");

        if (crc_get1 == crc_get2) {

            printf("Both two packets have the same FCS\n\n");

            printf("Error correction begins\n\n");

            crc_get = crc_get1;

            /* -------------------------------------------------------------------------- */
            /* --- STAGE : Decoding ---------------------- */


            uint8_t  payload1[BUF_SIZE];   /*!> buffer containing the payload */

            uint16_t size1; //json数据包里自带的，但mqtt event没有
            size1 = b64_to_bin(str1, strlen(str1), payload1, sizeof payload1); //与net_downlink相似，都是接收到data，故都用b64_to_bin
            printf("Copy_1 of data: %s\n", str1);
            delete[] str1;


            uint8_t  payload2[BUF_SIZE];   /*!> buffer containing the payload */
            uint16_t size2; //json数据包里自带的，但mqtt event没有
            size2 = b64_to_bin(str2, strlen(str2), payload2, sizeof payload2); //与net_downlink相似，都是接收到data，故都用b64_to_bin
            printf("Copy_2 of data: %s\n", str2);
            delete[] str2;


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


            char* Hexstring1 = new char[BUF_SIZE];
            memset(Hexstring1, 0, BUF_SIZE * sizeof(char));
            Uint2Char(payload1, Hexstring1, size);
            /* 测试代码
            printf("M's: %s\n", Hexstring1);
            */

            char* Hexstring2 = new char[BUF_SIZE];
            memset(Hexstring2, 0, BUF_SIZE * sizeof(char));
            Uint2Char(payload2, Hexstring2, size);
            /* 测试代码
            printf("M'r: %s\n", Hexstring2);
            */


            /* -------------------------------------------------------------------------- */
            /* --- STAGE : 十六进制字符串转二进制字符串 ---------------------- */ //https://blog.csdn.net/weixin_30279751/article/details/95437814

            char* Binarystring1 = new char[BUF_SIZE];
            memset(Binarystring1, 0, BUF_SIZE * sizeof(char));
            char* Binarystring2 = new char[BUF_SIZE];
            memset(Binarystring2, 0, BUF_SIZE * sizeof(char));

            Hex2Bin(Hexstring1, Binarystring1);
            Hex2Bin(Hexstring2, Binarystring2);
            delete[] Hexstring1;
            delete[] Hexstring2;

            /* -------------------------------------------------------------------------- */
            /* --- STAGE : 二进制字符串异或 ---------------------- */


            char* Binarystring3 = new char[BUF_SIZE]; //Merged error mask / Ambiguity vectors / Va
            memset(Binarystring3, 0, BUF_SIZE * sizeof(char));

            if (OZ_bin_xor(Binarystring1, Binarystring2, Binarystring3) != 0) //TODO: Majority voting / more than two copies
            {
                printf("函数出错！\n");
                return 1;
            }

            /* -------------------------------------------------------------------------- */
            /* --- STAGE : GetCandidate ---------------------- */
            /* -------------------------------------------------------------------------- */
            /* --- STAGE : CRC ---------------------- */


            char* mch = new char[BUF_SIZE];
            memset(mch, 0, BUF_SIZE * sizeof(char));
            if (rssi1>rssi2) {

                strcpy(mch, Binarystring1);

            }
            else {

                strcpy(mch, Binarystring2);

            }

            delete[] Binarystring1;
            delete[] Binarystring2;
            /* 测试代码
             printf("MCH: %s\n", mch);
             */
            char* crc = new char[BUF_SIZE];
            memset(crc, 0, BUF_SIZE * sizeof(char));
            sprintf(crc, "0x%04X", crc_get);
            printf("Processed CRC: %s\n", crc);
            int crc_int = 0;
            sscanf(crc, "%X", &crc_int); //用sscanf而不是atoi的原因是虽然linux有atoi，但是crc最前面的0还是没了
            delete[] crc;
            /* 测试代码
            printf("CRC int: %x\n", crc_int);
            */
            int i = 0;
            strcpy(s, Binarystring3);
            delete[] Binarystring3;
            /* 测试代码
            printf("Mask: %s\n", s);
            */

            int Hamming_weight_max = 30; //预设的最多纠错比特位数量
            if (Hamming_weight_now > Hamming_weight_max) {

                printf("%s: %d\n", "Hamming weight is larger than the max number", Hamming_weight_max);
                printf("This program will be shut down!\n");
                return 0;

            } {
                /* 测试代码
                printf("Hamming Weight: %d\n", Hamming_weight_now);
                */
            }

            char* fakeresult = new char[BUF_SIZE]; //每次candidate与mch异或的中间产值
            memset(fakeresult, 0, BUF_SIZE * sizeof(char));

            char* realresult = new char[BUF_SIZE]; //符合CRC校验的fakeresult
            memset(realresult, 0, BUF_SIZE * sizeof(char));
            int total_number = 0; //一共运行的次数
            int pass_crc = 0; //符合CRC校验的次数


            while (s[i])
                d[i++] = '0';

            outmystr(i - 1, mch, crc_int, fakeresult, realresult, size, pass_crc, total_number); //TODO: 按照hamming weight递增的方法产生error candidate
            delete[] mch;
            delete[] fakeresult;

            if (strlen(realresult) == 0) {
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


            char* Hexstring4 = new char[BUF_SIZE]; //char类型的PHYPayload
            memset(Hexstring4, 0, BUF_SIZE * sizeof(char));
            Bin2Hex(realresult, Hexstring4);
            delete[] realresult;
            /* 测试代码
            printf("RealresultHex: %s\n", Hexstring4);
            */

            /* -------------------------------------------------------------------------- */
            /* --- STAGE : Encoding ---------------------- */

            uint8_t* Hexstring4_uint8 = new uint8_t[BUF_SIZE];
            memset(Hexstring4_uint8, 0, BUF_SIZE * sizeof(uint8_t));


            Char2Uint(Hexstring4, Hexstring4_uint8);
            delete[] Hexstring4;


            uint8_t* data_up_uint8 = new uint8_t[BUF_SIZE]; //不用太大， 因为原代码里的buff_up不止装的data所以很大
            memset(data_up_uint8, 0, BUF_SIZE * sizeof(uint8_t));


            bin_to_b64(Hexstring4_uint8, size, (char*)(data_up_uint8), 341);
            delete[] Hexstring4_uint8;

            char* data_up = new char[BUF_SIZE]; //char类型的PHYPayload，即"data"里的字符串值
            memset(data_up, 0, BUF_SIZE * sizeof(char));
            strcpy(data_up, (char*)(data_up_uint8));
            printf("Corrected data: %s\n", data_up);
            delete[] data_up_uint8;

            /* 测试代码
            uint16_t    payload_crc16_calc;
            payload_crc16_calc = sx1302_lora_payload_crc(Hexstring4_uint8, size);
            printf("FixedPayload CRC (0x%04X)\n", payload_crc16_calc);
            */


            /* -------------------------------------------------------------------------- */
            /* --- STAGE : 新构造data_up的替换进buffer1_inter里的data部分 ---------------------- */
            //TODO: 解决多数据包同时上行情况

            JSON_Value* root_val = NULL;
            JSON_Object* first_obj = NULL;
            JSON_Array* rxpk_array = NULL;

            cJSON* json = NULL;
            cJSON* arrayItem = NULL;
            cJSON* object = NULL;
            cJSON* item = NULL;
        	
            char* buffer_inter = new char[BUF_SIZE]; //作为json字符串bufferi_inter的中间变量
            memset(buffer_inter, 0, BUF_SIZE * sizeof(char));

            char* buffer_inter_uint_char = new char[BUF_SIZE]; //json字符串的uint8_t值用char表示（前24个字符缺陷，第24个字符开始修改了）
            memset(buffer_inter_uint_char, 0, BUF_SIZE * sizeof(char));

            char buffer_send_first_part_char[BUF_SIZE] = { 0 }; //json字符串的uint8_t值用char表示的前24个字符

            char buffer_send_last_part_char[BUF_SIZE] = { 0 };  //json字符串的uint8_t值用char表示的第24个字符开始的部分

            uint8_t* buffer_send = new uint8_t[BUF_SIZE];  //json字符串的uint8_t值
            memset(buffer_send, 0, BUF_SIZE * sizeof(uint8_t));

            if (rssi1 >= rssi2) {


                /* -------------------------------------------------------------------------- */
                /* --- STAGE : 替换data_up ---------------------- */

                strncpy(buffer1_inter + FindFirstSubchar(buffer1_inter, report2) + 6, data_up, strlen(data_up)); //https://blog.csdn.net/zmhawk/article/details/44600075

            	/*测试代码 TODO: JSON serialization
            	root_val = json_parse_string_with_comments((const char*)(buffer_uint1 + 12));
                rxpk_array = json_object_get_array(json_value_get_object(root_val), "rxpk");
                first_obj = json_array_get_object(rxpk_array, 0);
                json_object_set_string(first_obj, "data", data_up);
                buffer1_inter = json_serialize_to_string(root_val);
                puts(buffer1_inter);
				*/

                json = cJSON_Parse((const char*)(buffer_uint1 + 12));
                arrayItem = cJSON_GetObjectItem(json, "rxpk");
                object = cJSON_GetArrayItem(arrayItem, 0);
                item = cJSON_GetObjectItem(object, "data");
                printf("data: %s\n", item->valuestring);
                item = cJSON_GetObjectItem(object, "stat");
                printf("stat: %d\n", item->valueint);
            	
                /* -------------------------------------------------------------------------- */
                /* --- STAGE : 更改stat从-1到1 ---------------------- */

                deleteChar(buffer1_inter, FindFirstSubchar(buffer1_inter, report1) + 5);
                buff_index1--;

                /* -------------------------------------------------------------------------- */
				/* --- STAGE : 构造出前24个字符缺陷的buffer_inter_uint_char ---------------------- */

                strcpy(buffer_inter, buffer1_inter);
                uint8_t* buffer_inter_uint = (uint8_t*)(buffer_inter - 12); //json字符串转化为uint8值（导致uint8_t值前24个字符缺陷）
                Uint2Char(buffer_inter_uint, buffer_inter_uint_char, buff_index1);
            	
                /* -------------------------------------------------------------------------- */
                /* --- STAGE : 将buff_i的前24个字符(必然不会被修改的部分) 与buffer_inter_uint_char的第24个字符开始的部分(修改了的部分) 组合起来，转换为uint8_t的buffer_send ---------------------- */


                strncpy(buffer_send_first_part_char, buffer1, 24);
                buffer_send_first_part_char[strlen(buffer_send_first_part_char)] = '\0';
                strncpy(buffer_send_last_part_char, buffer_inter_uint_char + 24, strlen(buffer_inter_uint_char) - 24);
                buffer_send_last_part_char[strlen(buffer_send_last_part_char)] = '\0';

                strcat(buffer_send_first_part_char, buffer_send_last_part_char);
                Char2Uint(buffer_send_first_part_char, buffer_send);

                printf("buffer_send: ");
                for (int count = 0; count < buff_index1; count++) {
                    printf("%02X", buffer_send[count]);
                }
                printf("\n\n");

                //delete[] rssis1;
                //delete[] rssis2;

                delete[] data_up;
                delete[] buffer_inter;
                delete[] buffer_inter_uint_char;

            }
            else {

                strncpy(buffer2_inter + FindFirstSubchar(buffer2_inter, report2) + 6, data_up, strlen(data_up));
                deleteChar(buffer2_inter, FindFirstSubchar(buffer2_inter, report1) + 5);
                buff_index2--;
                strcpy(buffer_inter, buffer2_inter);
                uint8_t* buffer_inter_uint = (uint8_t*)(buffer_inter - 12);
                Uint2Char(buffer_inter_uint, buffer_inter_uint_char, buff_index2);


                strncpy(buffer_send_first_part_char, buffer2, 24);
                buffer_send_first_part_char[strlen(buffer_send_first_part_char)] = '\0';
                strncpy(buffer_send_last_part_char, buffer_inter_uint_char + 24, strlen(buffer_inter_uint_char) - 24);
                buffer_send_last_part_char[strlen(buffer_send_last_part_char)] = '\0';

                strcat(buffer_send_first_part_char, buffer_send_last_part_char);
                Char2Uint(buffer_send_first_part_char, buffer_send);

                printf("buffer_send: ");
                for (int count = 0; count < buff_index2; count++) {
                    printf("%02X", buffer_send[count]);
                }
                printf("\n\n");


                delete[] data_up;
                delete[] buffer_inter;
                delete[] buffer_inter_uint_char;

            }



        }
        else {

            //TOTDO: 两个包CRC不同，说明不是同一个数据包的副本，无法改错
            printf("Both two packets do not have the same FCS, no operation will be taken\n");

            printf("buffer_send1: ");
            for (int count = 0; count < buff_index1; count++) {
                printf("%02X", buffer1_inter_uint[count]);
            }
            printf("\n\n");

            printf("buffer_send2: ");
            for (int count = 0; count < buff_index2; count++) {
                printf("%02X", buffer2_inter_uint[count]);
            }
            printf("\n\n");

        }

    }
    else {

        //TODO: 只要有一个没有错则不进行处理
        printf("At least one packet is crc correct, no operation will be taken\n\n");

        printf("buffer_send1: ");
        for (int count = 0; count < buff_index1; count++) {
            printf("%02X", buffer1_inter_uint[count]);
        }
        printf("\n\n");

        printf("buffer_send2: ");
        for (int count = 0; count < buff_index2; count++) {
            printf("%02X", buffer2_inter_uint[count]);
        }
        printf("\n\n");

    }


    return 0;
}