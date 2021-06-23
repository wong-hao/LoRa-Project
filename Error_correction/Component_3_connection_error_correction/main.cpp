#include <functional>

#include"Component_1_fakesend/inc/header_1_1.h"
#include"Component_1_getCandidate/inc/header_1_2.h"
#include"Component_1_getMast/inc/header_1_3.h"

#include "Component_2_server_side/inc/header_2.h"

#include "tools/inc/payload_crc.h"
#include "tools/inc/parson.h"

#include"tools/inc/payload_diff.h"

#include"Component_3_connectton_error_correction/inc/header_3.h"

#include "tools/inc/base64.h"
#include "tools/inc/cJSON.h"


extern char s[BUF_SIZE], d[BUF_SIZE]; //s是Merged error mask；d是Error candidate pattern
extern int buff_index;


class Buffer
{
public:
    char* data;
    int index;
    uint8_t uint[BUF_SIZE];
    char* inter;
    uint8_t* inter_uint;
    char* inter_uint_char;
    char send_first_part_char[BUF_SIZE];
    char send_last_part_char[BUF_SIZE];
    uint8_t* send;

    uint8_t  payload[BUF_SIZE];   /*!> buffer containing the payload */
    uint16_t size; //json数据包里自带的，但mqtt event没有
    char* Hexstring;
    char* Binarystring;
    uint8_t* Hexstring_uint8;

};


int main()

{
    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 模拟server_side接收到的buffer数据进行试验---------------------- */
    //TODO: 取多个真实值实验
    Buffer buffer1{};
    Buffer buffer2{};
    Buffer buffer{};

    buffer1.data = new char[BUF_SIZE];
    memset(buffer1.data, 0, BUF_SIZE * sizeof(char));
    buffer2.data = new char[BUF_SIZE];
    memset(buffer2.data, 0, BUF_SIZE * sizeof(char));

    strcpy(buffer1.data, "02AA1A000016C001FF10D3F67B227278706B223A5B7B226A766572223A312C22746D7374223A3830323838333330392C2274696D65223A22323032312D30342D31335430323A34383A30372E3030303030303030305A222C22746D6D73223A313330323331373238373030302C226368616E223A302C2272666368223A302C2266726571223A3438362E3330303030302C226D6964223A31302C2273746174223A2D312C22637263223A33333735302C226D6F6475223A224C4F5241222C2264617472223A225346374257313235222C22636F6472223A22342F35222C227273736973223A2D37372C226C736E72223A2D362E352C22666F6666223A2D3235372C2272737369223A2D37312C2273697A65223A31382C2264617461223A225969515442435741435441435A455878472B614E4A7A4E6B227D2C7B226A766572223A312C22746D7374223A3830323838333331312C2274696D65223A22323032312D30342D31335430323A34383A30372E3030303030303030305A222C22746D6D73223A313330323331373238373030302C226368616E223A352C2272666368223A312C2266726571223A3438372E3330303030302C226D6964223A20382C2273746174223A312C22637263223A33333735302C226D6F6475223A224C4F5241222C2264617472223A225346374257313235222C22636F6472223A22342F35222C227273736973223A2D31302C226C736E72223A31332E322C22666F6666223A2D3236322C2272737369223A2D392C2273697A65223A31382C2264617461223A2251415154424361414351414356306278472B614E4A7A4E6B227D5D7D");
    strcpy(buffer2.data, "02AA1A000016C001FF10D3F77B227278706B223A5B7B226A766572223A312C22746D7374223A3830303933323837322C2274696D65223A22323032312D30342D31335430323A34383A30372E3030303030303030305A222C22746D6D73223A313330323331373238373030302C226368616E223A302C2272666368223A302C2266726571223A3438362E3330303030302C226D6964223A31312C2273746174223A2D312C22637263223A33333735302C226D6F6475223A224C4F5241222C2264617472223A225346374257313235222C22636F6472223A22342F35222C227273736973223A2D39312C226C736E72223A2D382E352C22666F6666223A2D3238372C2272737369223A2D38332C2273697A65223A31382C2264617461223A2251415154424F627342564148416B5078472B614E4A7A4E6B227D2C7B226A766572223A312C22746D7374223A3830303933323937372C2274696D65223A22323032312D30342D31335430323A34383A30372E3030303030303030305A222C22746D6D73223A313330323331373238373030302C226368616E223A352C2272666368223A312C2266726571223A3438372E3330303030302C226D6964223A20382C2273746174223A312C22637263223A33333735302C226D6F6475223A224C4F5241222C2264617472223A225346374257313235222C22636F6472223A22342F35222C227273736973223A2D33312C226C736E72223A31332E382C22666F6666223A2D3236392C2272737369223A2D33312C2273697A65223A31382C2264617461223A2251415154424361414351414356306278472B614E4A7A4E6B227D5D7D");

    buffer1.index = strlen(buffer1.data) / 2;
    buffer2.index = strlen(buffer2.data) / 2;

    buffer1.uint[BUF_SIZE] = {0};
    buffer2.uint[BUF_SIZE] = {0};

    Char2Uint(buffer1.data, buffer1.uint);
    Char2Uint(buffer2.data, buffer2.uint);

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 对中间数据buffer_inter纠错---------------------- */

    buffer1.inter = (char*)(buffer1.uint + buff_index); //接收到的Upstream JSON data structure
    buffer2.inter = (char*)(buffer2.uint + buff_index);

    printf("buffer1.inter: %s\n", buffer1.inter);
    printf("\n");
    printf("buffer2.inter: %s\n", buffer2.inter);
    printf("\n");

    buffer1.inter_uint = (uint8_t*)(buffer1.inter - buff_index);
    buffer2.inter_uint = (uint8_t*)(buffer2.inter - buff_index);

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 找到上行数据中需要的属性的值 ---------------------- */
    //TODO: 解决多数据包同时上行情况 (重复数据包接收是因为距离太近)
    //https://forum.rakwireless.com/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/3980/3?u=haowong
    //https://forum.chirpstack.io/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/10886/2?u=shirou_emiya

    Rxpk rxpk1{};
    Rxpk rxpk2{};

    rxpk1.time = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer1.uint + buff_index))), "rxpk"), 0), "time");
    rxpk2.time = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer2.uint + buff_index))), "rxpk"), 0), "time");

    rxpk1.stat = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer1.uint + buff_index))), "rxpk"), 0), "stat"));
    rxpk2.stat = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer2.uint + buff_index))), "rxpk"), 0), "stat"));


    rxpk1.crc_get = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer1.uint + buff_index))), "rxpk"), 0), "crc"));
    rxpk2.crc_get = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer2.uint + buff_index))), "rxpk"), 0), "crc"));
    unsigned int crc_get = 0;

    rxpk1.str = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer1.uint + buff_index))), "rxpk"), 0), "data");
    rxpk2.str = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer2.uint + buff_index))), "rxpk"), 0), "data");

    rxpk1.rssi = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer1.uint + buff_index))), "rxpk"), 0), "rssi"));
    rxpk2.rssi = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer2.uint + buff_index))), "rxpk"), 0), "rssi"));


    /*测试代码
    printf("rxpk1.stat: %d\n", rxpk1.stat);
    printf("rxpk1.crc_get: %d\n", rxpk1.crc_get);
    printf("rxpk1.str: %s\n", rxpk1.str);
    printf("rssis1: %d\n", rssis1);
    printf("time1: %s\n", time1);
    */


    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 当两个上行数据都错且crc值相同时进行纠错 ---------------------- */


    if ((rxpk1.stat== -1) && (rxpk2.stat == -1)) {


        printf("Both two packets are crc incorrect\n");

        if (rxpk1.crc_get == rxpk2.crc_get) {

            printf("Both two packets have the same FCS\n\n");

            printf("Error correction begins\n\n");

            crc_get = rxpk1.crc_get;

            /* -------------------------------------------------------------------------- */
            /* --- STAGE : Decoding ---------------------- */

            buffer1.payload[BUF_SIZE] = {0};

            buffer1.size = b64_to_bin(rxpk1.str, strlen(rxpk1.str), buffer1.payload, sizeof buffer1.payload); //与net_downlink相似，都是接收到data，故都用b64_to_bin
            printf("Copy_1 of data: %s\n", rxpk1.str);
            delete[] rxpk1.str;

            buffer2.payload[BUF_SIZE] = {0};

            buffer2.size = b64_to_bin(rxpk2.str, strlen(rxpk2.str), buffer2.payload, sizeof buffer2.payload); //与net_downlink相似，都是接收到data，故都用b64_to_bin
            printf("Copy_2 of data: %s\n", rxpk2.str);
            delete[] rxpk2.str;


            uint16_t size;

            if (buffer1.size == buffer2.size) {
                size = buffer1.size;
            }
            else {
                printf("Error: length1 is not equal to length2. This program will be shut down!");
                return 0;
            }

            int Hamming_weight_now = 0;
            getNe(buffer1.payload, buffer2.payload, size, Hamming_weight_now);

            /* -------------------------------------------------------------------------- */
            /* --- STAGE : uint8_t转char ---------------------- */ //https://bbs.csdn.net/topics/390141308


            buffer1.Hexstring = new char[BUF_SIZE];
            memset(buffer1.Hexstring, 0, BUF_SIZE * sizeof(char));
            Uint2Char(buffer1.payload, buffer1.Hexstring, size);
            /* 测试代码
            printf("M's: %s\n", buffer1.Hexstring);
            */

            buffer2.Hexstring = new char[BUF_SIZE];
            memset(buffer2.Hexstring, 0, BUF_SIZE * sizeof(char));
            Uint2Char(buffer2.payload, buffer2.Hexstring, size);
            /* 测试代码
            printf("M'r: %s\n", buffer2.Hexstring);
            */


            /* -------------------------------------------------------------------------- */
            /* --- STAGE : 十六进制字符串转二进制字符串 ---------------------- */ //https://blog.csdn.net/weixin_30279751/article/details/95437814

            buffer1.Binarystring = new char[BUF_SIZE];
            memset(buffer1.Binarystring, 0, BUF_SIZE * sizeof(char));
            buffer2.Binarystring = new char[BUF_SIZE];
            memset(buffer2.Binarystring, 0, BUF_SIZE * sizeof(char));

            Hex2Bin(buffer1.Hexstring, buffer1.Binarystring);
            Hex2Bin(buffer2.Hexstring, buffer2.Binarystring);
            delete[] buffer1.Hexstring;
            delete[] buffer2.Hexstring;

            /* -------------------------------------------------------------------------- */
            /* --- STAGE : 二进制字符串异或 ---------------------- */


            buffer.Binarystring = new char[BUF_SIZE]; //Merged error mask / Ambiguity vectors / Va
            memset(buffer.Binarystring, 0, BUF_SIZE * sizeof(char));

            if (OZ_bin_xor(buffer1.Binarystring, buffer2.Binarystring, buffer.Binarystring) != 0) //TODO: Majority voting / more than two copies
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
            if (rxpk1.rssi>rxpk2.rssi) {

                strcpy(mch, buffer1.Binarystring);

            }
            else {

                strcpy(mch, buffer2.Binarystring);

            }

            delete[] buffer1.Binarystring;
            delete[] buffer2.Binarystring;
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
            strcpy(s, buffer.Binarystring);
            delete[] buffer.Binarystring;
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


            buffer.Hexstring = new char[BUF_SIZE]; //char类型的PHYPayload
            memset(buffer.Hexstring, 0, BUF_SIZE * sizeof(char));
            Bin2Hex(realresult, buffer.Hexstring);
            delete[] realresult;
            /* 测试代码
            printf("RealresultHex: %s\n", buffer.Hexstring);
            */

            /* -------------------------------------------------------------------------- */
            /* --- STAGE : Encoding ---------------------- */

            buffer.Hexstring_uint8 = new uint8_t[BUF_SIZE];
            memset(buffer.Hexstring_uint8, 0, BUF_SIZE * sizeof(uint8_t));


            Char2Uint(buffer.Hexstring, buffer.Hexstring_uint8);
            delete[] buffer.Hexstring;


            uint8_t* data_up_uint8 = new uint8_t[BUF_SIZE];
            memset(data_up_uint8, 0, BUF_SIZE * sizeof(uint8_t));


            bin_to_b64(buffer.Hexstring_uint8, size, (char*)(data_up_uint8), 341);
            delete[] buffer.Hexstring_uint8;

            char* data_up = new char[BUF_SIZE]; //char类型的PHYPayload，即"data"里的字符串值
            memset(data_up, 0, BUF_SIZE * sizeof(char));
            strcpy(data_up, (char*)(data_up_uint8));
            printf("Corrected data: %s\n", data_up);
            delete[] data_up_uint8;

            /* 测试代码
            uint16_t    payload_crc16_calc;
            payload_crc16_calc = sx1302_lora_payload_crc(buffer.Hexstring_uint8, size);
            printf("FixedPayload CRC (0x%04X)\n", payload_crc16_calc);
            */


            /* -------------------------------------------------------------------------- */
            /* --- STAGE : 修改Upstream JSON data structure ---------------------- */
            //TODO: 解决多数据包同时上行情况
            buffer.inter = new char[BUF_SIZE]; //将bufferi_inter赋值buffer_inter给以后续处理
            memset(buffer.inter, 0, BUF_SIZE * sizeof(char));

            buffer.inter_uint_char = new char[BUF_SIZE]; //需要发送的数据的char形式（此时前12-byte header有缺陷，第12 byte后为修改后的Upstream JSON data structure）
            memset(buffer.inter_uint_char, 0, BUF_SIZE * sizeof(char));

            buffer.send_first_part_char[BUF_SIZE] = { 0 }; //12-byte header

            buffer.send_last_part_char[BUF_SIZE] = { 0 };  //修改后的Upstream JSON data structure

            buffer.send = new uint8_t[BUF_SIZE];  //需要发送的数据 (原始uint8形式)
            memset(buffer.send, 0, BUF_SIZE * sizeof(uint8_t));

            if (rxpk1.rssi >= rxpk2.rssi) {


                /* -------------------------------------------------------------------------- */
                /* --- STAGE : 将Upstream JSON data structure的"data" field里面的数据使用修改后的data_up覆盖 ---------------------- */

                strncpy(buffer1.inter + FindFirstSubchar(buffer1.inter, "data") + 6, data_up, strlen(data_up)); //https://blog.csdn.net/zmhawk/article/details/44600075

            	/*测试代码 TODO: JSON serialization
            	 
                JSON_Value* root_val = NULL;
				JSON_Object* first_obj = NULL;
				JSON_Array* rxpk_array = NULL;

				cJSON* json = NULL;
				cJSON* arrayItem = NULL;
				cJSON* object = NULL;
				cJSON* item = NULL;
            
            	root_val = json_parse_string_with_comments((const char*)(buffer1.uint + buff_index));
                rxpk_array = json_object_get_array(json_value_get_object(root_val), "rxpk");
                first_obj = json_array_get_object(rxpk_array, 0);
                json_object_set_string(first_obj, "data", data_up);
                buffer1.inter = json_serialize_to_string(root_val);
                puts(buffer1.inter);

                json = cJSON_Parse((const char*)(buffer1.uint + buff_index));
                arrayItem = cJSON_GetObjectItem(json, "rxpk");
                object = cJSON_GetArrayItem(arrayItem, 0);
                item = cJSON_GetObjectItem(object, "data");
                printf("data: %s\n", item->valuestring);
                item = cJSON_GetObjectItem(object, "stat");
                printf("stat: %d\n", item->valueint);
                buffer1.inter = cJSON_PrintUnformatted(json);
                puts(buffer1.inter);
				*/

                /* -------------------------------------------------------------------------- */
                /* --- STAGE : 更改stat从-1到1 ---------------------- */

                deleteChar(buffer1.inter, FindFirstSubchar(buffer1.inter, "stat") + 5);
                buffer1.index--;

                /* -------------------------------------------------------------------------- */
				/* --- STAGE : 构造出前12-byte header缺陷的buffer_inter_uint_char ---------------------- */

                strcpy(buffer.inter, buffer1.inter);
                buffer.inter_uint = (uint8_t*)(buffer.inter - buff_index); //json字符串转化为uint8值（导致uint8_t值前12-byte缺陷）
                Uint2Char(buffer.inter_uint, buffer.inter_uint_char, buffer1.index);
            	
                /* -------------------------------------------------------------------------- */
                /* --- STAGE : 将buff_i的前12-byte(必然不会被修改的header部分) 与buffer_inter_uint_char的第12 byte开始的部分(修改后的Upstream JSON data structure) 组合起来，转换为uint8_t的buffer_send ---------------------- */


                strncpy(buffer.send_first_part_char, buffer1.data, buff_index*2);
                buffer.send_first_part_char[strlen(buffer.send_first_part_char)] = '\0';
                strncpy(buffer.send_last_part_char, buffer.inter_uint_char + buff_index*2, strlen(buffer.inter_uint_char) - buff_index*2);
                buffer.send_last_part_char[strlen(buffer.send_last_part_char)] = '\0';

                strcat(buffer.send_first_part_char, buffer.send_last_part_char);
                Char2Uint(buffer.send_first_part_char, buffer.send);

                printf("buffer.send: ");
                for (int count = 0; count < buffer1.index; count++) {
                    printf("%02X", buffer.send[count]);
                }
                printf("\n\n");

                //delete[] rssis1;
                //delete[] rssis2;

                delete[] data_up;
                delete[] buffer.inter;
                delete[] buffer.inter_uint_char;

            }
            else {

                strncpy(buffer2.inter + FindFirstSubchar(buffer2.inter, "data") + 6, data_up, strlen(data_up));
                deleteChar(buffer2.inter, FindFirstSubchar(buffer2.inter, "stat") + 5);
                buffer2.index--;
                strcpy(buffer.inter, buffer2.inter);
                buffer.inter_uint = (uint8_t*)(buffer.inter - buff_index);
                Uint2Char(buffer.inter_uint, buffer.inter_uint_char, buffer2.index);


                strncpy(buffer.send_first_part_char, buffer2.data, buff_index*2);
                buffer.send_first_part_char[strlen(buffer.send_first_part_char)] = '\0';
                strncpy(buffer.send_last_part_char, buffer.inter_uint_char + buff_index*2, strlen(buffer.inter_uint_char) - buff_index*2);
                buffer.send_last_part_char[strlen(buffer.send_last_part_char)] = '\0';

                strcat(buffer.send_first_part_char, buffer.send_last_part_char);
                Char2Uint(buffer.send_first_part_char, buffer.send);

                printf("buffer.send: ");
                for (int count = 0; count < buffer2.index; count++) {
                    printf("%02X", buffer.send[count]);
                }
                printf("\n\n");


                delete[] data_up;
                delete[] buffer.inter;
                delete[] buffer.inter_uint_char;

            }



        }
        else {

            //TOTDO: 两个包CRC不同，说明不是同一个数据包的副本，无法改错
            printf("Both two packets do not have the same FCS, no operation will be taken\n");

            printf("buffer_send1: ");
            for (int count = 0; count < buffer1.index; count++) {
                printf("%02X", buffer1.inter_uint[count]);
            }
            printf("\n\n");

            printf("buffer_send2: ");
            for (int count = 0; count < buffer2.index; count++) {
                printf("%02X", buffer2.inter_uint[count]);
            }
            printf("\n\n");

        }

    }
    else {

        //TODO: 只要有一个没有错则不进行处理
        printf("At least one packet is crc correct, no operation will be taken\n\n");

        printf("buffer_send1: ");
        for (int count = 0; count < buffer1.index; count++) {
            printf("%02X", buffer1.inter_uint[count]);
        }
        printf("\n\n");

        printf("buffer_send2: ");
        for (int count = 0; count < buffer2.index; count++) {
            printf("%02X", buffer2.inter_uint[count]);
        }
        printf("\n\n");

    }


    return 0;
}