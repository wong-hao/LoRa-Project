#include"header_1_1.h"
#include"header_1_2.h"
#include "header_1_3.h"

#include "header_2.h"

#include "payload_crc.h"
#include "parson.h"

#include"payload_diff.h"

#include"header_3.h"

#include "base64.h"
#include "cJSON.h"

extern int sock_up;

int main()

{
    char* buff_up_char1 = "02AA1A000016C001FF10D3F67B227278706B223A5B7B226A766572223A312C22746D7374223A3830323838333330392C2274696D65223A22323032312D30342D31335430323A34383A30372E3030303030303030305A222C22746D6D73223A313330323331373238373030302C226368616E223A302C2272666368223A302C2266726571223A3438362E3330303030302C226D6964223A31302C2273746174223A2D312C22637263223A33333735302C226D6F6475223A224C4F5241222C2264617472223A225346374257313235222C22636F6472223A22342F35222C227273736973223A2D37372C226C736E72223A2D362E352C22666F6666223A2D3235372C2272737369223A2D37312C2273697A65223A31382C2264617461223A225969515442435741435441435A455878472B614E4A7A4E6B227D2C7B226A766572223A312C22746D7374223A3830323838333331312C2274696D65223A22323032312D30342D31335430323A34383A30372E3030303030303030305A222C22746D6D73223A313330323331373238373030302C226368616E223A352C2272666368223A312C2266726571223A3438372E3330303030302C226D6964223A20382C2273746174223A312C22637263223A33333735302C226D6F6475223A224C4F5241222C2264617472223A225346374257313235222C22636F6472223A22342F35222C227273736973223A2D31302C226C736E72223A31332E322C22666F6666223A2D3236322C2272737369223A2D392C2273697A65223A31382C2264617461223A2251415154424361414351414356306278472B614E4A7A4E6B227D5D7D";
    char* buff_up_char2 = "02AA1A000016C001FF10D3F77B227278706B223A5B7B226A766572223A312C22746D7374223A3830303933323837322C2274696D65223A22323032312D30342D31335430323A34383A30372E3030303030303030305A222C22746D6D73223A313330323331373238373030302C226368616E223A302C2272666368223A302C2266726571223A3438362E3330303030302C226D6964223A31312C2273746174223A2D312C22637263223A33333735302C226D6F6475223A224C4F5241222C2264617472223A225346374257313235222C22636F6472223A22342F35222C227273736973223A2D39312C226C736E72223A2D382E352C22666F6666223A2D3238372C2272737369223A2D38332C2273697A65223A31382C2264617461223A2251415154424F627342564148416B5078472B614E4A7A4E6B227D2C7B226A766572223A312C22746D7374223A3830303933323937372C2274696D65223A22323032312D30342D31335430323A34383A30372E3030303030303030305A222C22746D6D73223A313330323331373238373030302C226368616E223A352C2272666368223A312C2266726571223A3438372E3330303030302C226D6964223A20382C2273746174223A312C22637263223A33333735302C226D6F6475223A224C4F5241222C2264617472223A225346374257313235222C22636F6472223A22342F35222C227273736973223A2D33312C226C736E72223A31332E382C22666F6666223A2D3236392C2272737369223A2D33312C2273697A65223A31382C2264617461223A2251415154424361414351414356306278472B614E4A7A4E6B227D5D7D";
    char* buff_up_char3 = "02AA1A000016C001FF10D3F77B227278706B223A5B7B226A766572223A312C22746D7374223A3830303933323837322C2274696D65223A22323032312D30342D31335430323A34383A30372E3030303030303030305A222C22746D6D73223A313330323331373238373030302C226368616E223A302C2272666368223A302C2266726571223A3438362E3330303030302C226D6964223A31312C2273746174223A2D312C22637263223A33333735302C226D6F6475223A224C4F5241222C2264617472223A225346374257313235222C22636F6472223A22342F35222C227273736973223A2D39312C226C736E72223A2D382E352C22666F6666223A2D3238372C2272737369223A2D38332C2273697A65223A31382C2264617461223A2251415154424F627562564148416B5078472B614E4A7A4E6B227D2C7B226A766572223A312C22746D7374223A3830303933323937372C2274696D65223A22323032312D30342D31335430323A34383A30372E3030303030303030305A222C22746D6D73223A313330323331373238373030302C226368616E223A352C2272666368223A312C2266726571223A3438372E3330303030302C226D6964223A20382C2273746174223A312C22637263223A33333735302C226D6F6475223A224C4F5241222C2264617472223A225346374257313235222C22636F6472223A22342F35222C227273736973223A2D33312C226C736E72223A31332E382C22666F6666223A2D3236392C2272737369223A2D33312C2273697A65223A31382C2264617461223A2251415154424361414351414356306278472B614E4A7A4E6B227D5D7D";
    char* buff_up_char4 = "02AA1A000016C001FF10D3F77B227278706B223A5B7B226A766572223A312C22746D7374223A3830303933323837322C2274696D65223A22323032312D30342D31335430323A34383A30372E3030303030303030305A222C22746D6D73223A313330323331373238373030302C226368616E223A302C2272666368223A302C2266726571223A3438362E3330303030302C226D6964223A31312C2273746174223A2D312C22637263223A33333735302C226D6F6475223A224C4F5241222C2264617472223A225346374257313235222C22636F6472223A22342F35222C227273736973223A2D39312C226C736E72223A2D382E352C22666F6666223A2D3238372C2272737369223A2D38332C2273697A65223A31382C2264617461223A2251415154424F627342564148416B6178472B614E4A7A4E6B227D2C7B226A766572223A312C22746D7374223A3830303933323937372C2274696D65223A22323032312D30342D31335430323A34383A30372E3030303030303030305A222C22746D6D73223A313330323331373238373030302C226368616E223A352C2272666368223A312C2266726571223A3438372E3330303030302C226D6964223A20382C2273746174223A312C22637263223A33333735302C226D6F6475223A224C4F5241222C2264617472223A225346374257313235222C22636F6472223A22342F35222C227273736973223A2D33312C226C736E72223A31332E382C22666F6666223A2D3236392C2272737369223A2D33312C2273697A65223A31382C2264617461223A2251415154424361414351414356306278472B614E4A7A4E6B227D5D7D";

    printf("Please enter the mode you want to try: PC or APC\n");
    char choice[20] = {0};
    cin>>choice;

    if(strcmp(choice, "PC")==0){
        /* -------------------------------------------------------------------------- */
        /* --- STAGE : 模拟server_side接收到的buffer数据进行试验---------------------- */
        //TODO: 取多个真实值实验

        int buffer_num = 2;
        Buffer buffer_array[buffer_num];


        BufferSend buffer{};

        for(int i=0; i<=buffer_num-1; i++){
            buffer_array[i].data = new char[BUF_SIZE];
            memset(buffer_array[i].data, 0, BUF_SIZE * sizeof(char));
        }

        buffer_array[0].setData(buff_up_char1);
        buffer_array[1].setData(buff_up_char2);

        for(int i=0; i<=buffer_num-1; i++){
            buffer_array[i].setIndex();
            buffer_array[i].uint[BUF_SIZE] = {0};
            buffer_array[i].setUint();
        }

        /* -------------------------------------------------------------------------- */
        /* --- STAGE : 对中间数据buffer_inter纠错---------------------- */

        for(int i=0; i<=buffer_num-1; i++){
            buffer_array[i].setInter(); //接收到的Upstream JSON data structure
            cout<<"buffer"<<i+1<<".inter: "<<buffer_array[i].inter<<endl;
            buffer_array[i].setInter_Uint();
        }

        /* -------------------------------------------------------------------------- */
        /* --- STAGE : 找到上行数据中需要的属性的值 ---------------------- */
        //TODO: 解决多数据包同时上行情况 (重复数据包接收是因为距离太近)
        //https://forum.rakwireless.com/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/3980/3?u=haowong
        //https://forum.chirpstack.io/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/10886/2?u=shirou_emiya

        Rxpk rxpk_array[buffer_num];


        for(int i=0; i<=buffer_num-1; i++){
            rxpk_array[i].setTime(buffer_array[i].uint,buffer_array->buff_index);
            rxpk_array[i].setStat(buffer_array[i].uint,buffer_array->buff_index);
            rxpk_array[i].setCrc_get(buffer_array[i].uint,buffer_array->buff_index);
            rxpk_array[i].setStr(buffer_array[i].uint,buffer_array->buff_index);
            rxpk_array[i].setRssi(buffer_array[i].uint,buffer_array->buff_index);
        }

        unsigned int crc_get = 0;

#if DEBUG
        printf("rxpk1.stat: %d\n", rxpk_array[0].stat);
    printf("rxpk1.crc_get: %d\n", rxpk_array[0].crc_get);
    printf("rxpk1.str: %s\n", rxpk_array[0].str);
    printf("rssi1: %d\n", rxpk_array[0].rssi);
    printf("time1: %s\n", rxpk_array[0].time);
#endif

        /* -------------------------------------------------------------------------- */
        /* --- STAGE : 当两个上行数据都错且crc值相同时进行纠错 ---------------------- */


        if (compareStat(rxpk_array, buffer_num)) {


            printf("Both two packets are crc incorrect\n");

            if (compareCRC(rxpk_array, buffer_num)) {

                printf("Both two packets have the same FCS\n\n");

                printf("Error correction begins\n\n");

                crc_get = rxpk_array[0].crc_get;

                /* -------------------------------------------------------------------------- */
                /* --- STAGE : Decoding ---------------------- */

                for(int i=0; i<=buffer_num-1; i++){
                    buffer_array[i].payload[BUF_SIZE] = {0};
                    buffer_array[i].setSize(rxpk_array[i].str); //与net_downlink相似，都是接收到data，故都用b64_to_bin
                    cout<<"copy"<<i+1<<" of data: "<<rxpk_array[i].str<<endl;
                    delete[] rxpk_array[i].str;
                }

                uint16_t size;

                if (buffer_array[0].size == buffer_array[1].size) {
                    size = buffer_array[0].size;
                }
                else {
                    printf("Error: length1 is not equal to length2. This program will be shut down!");
                    return 0;
                }

                int Hamming_weight_now = 0;
                getNe(buffer_array[0].payload, buffer_array[1].payload, size, Hamming_weight_now);

                /*测试代码
                printf("Hamming_weight_now: %d\n", Hamming_weight_now);
                */

                /* -------------------------------------------------------------------------- */
                /* --- STAGE : uint8_t转char ---------------------- */ //https://bbs.csdn.net/topics/390141308

                for(int i=0; i<=buffer_num-1; i++){
                    buffer_array[i].Hexstring = new char[BUF_SIZE];
                    memset(buffer_array[i].Hexstring, 0, BUF_SIZE * sizeof(char));

                    buffer_array[i].setHexstring();
                }


#if DEBUG
                printf("M's: %s\n", buffer1.Hexstring);
            printf("M'r: %s\n", buffer2.Hexstring);
#endif



                /* -------------------------------------------------------------------------- */
                /* --- STAGE : 十六进制字符串转二进制字符串 ---------------------- */ //https://blog.csdn.net/weixin_30279751/article/details/95437814

                for(int i=0; i<=buffer_num-1; i++){
                    buffer_array[i].Binarystring = new char[BUF_SIZE];
                    memset(buffer_array[i].Binarystring, 0, BUF_SIZE * sizeof(char));

                    buffer_array[i].setBinarystring();
                    delete[] buffer_array[i].Hexstring;
                }

                /* -------------------------------------------------------------------------- */
                /* --- STAGE : 二进制字符串异或 ---------------------- */

                buffer.Binarystring = new char[BUF_SIZE]; //Merged error mask / Ambiguity vectors / Va
                memset(buffer.Binarystring, 0, BUF_SIZE * sizeof(char));

                buffer.setBinarystring(buffer_array[0].Binarystring, buffer_array[1].Binarystring);

                /* -------------------------------------------------------------------------- */
                /* --- STAGE : GetCandidate ---------------------- */
                /* -------------------------------------------------------------------------- */
                /* --- STAGE : CRC ---------------------- */


                char* mch = new char[BUF_SIZE];
                memset(mch, 0, BUF_SIZE * sizeof(char));

                int index = compareRSSI(rxpk_array, buffer_num);//Selection Combining (SC)
                strcpy(mch, buffer_array[index].Binarystring);

                for(int loopcount=0; loopcount<=buffer_num-1; loopcount++){
                    delete[] buffer_array[loopcount].Binarystring;
                }

#if DEBUG
                printf("MCH: %s\n", mch);
#endif
                char* crc = new char[BUF_SIZE];
                memset(crc, 0, BUF_SIZE * sizeof(char));
                sprintf(crc, "0x%04X", crc_get);
                printf("Processed CRC: %s\n", crc);
                int crc_int = 0;
                sscanf(crc, "%X", &crc_int); //用sscanf而不是atoi的原因是虽然linux有atoi，但是crc最前面的0还是没了
                delete[] crc;
#if DEBUG
                printf("CRC int: %x\n", crc_int);
                printf("Mask: %s\n", s);
#endif

                int Hamming_weight_max = 30; //预设的最多纠错比特位数量
                if (Hamming_weight_now > Hamming_weight_max) {

                    printf("%s: %d\n", "Hamming weight is larger than the max number", Hamming_weight_max);
                    printf("This program will be shut down!\n");
                    return 0;

                }

#if DEBUG
                printf("Hamming_weight_now: %d\n", Hamming_weight_now);
#endif


                char* fakeresult = new char[BUF_SIZE]; //每次candidate与mch异或的中间产值
                memset(fakeresult, 0, BUF_SIZE * sizeof(char));

                char* realresult = new char[BUF_SIZE]; //符合CRC校验的fakeresult
                memset(realresult, 0, BUF_SIZE * sizeof(char));
                int total_number = 0; //一共运行的次数
                int pass_crc = 0; //符合CRC校验的次数

                struct timespec startTime;
                clock_gettime(CLOCK_REALTIME, &startTime);

                if(Hamming_weight_now <= Hamming_weight_max/2){
                    incremental_correct(buffer.Binarystring, mch, Hamming_weight_now, crc_int, fakeresult, realresult, size, pass_crc, total_number, startTime);
                }else{
                    correct(buffer.Binarystring, mch, Hamming_weight_now, crc_int, fakeresult, realresult, size, pass_crc, total_number, startTime);
                }

                struct timespec endTime;
                clock_gettime(CLOCK_REALTIME, &endTime);

                struct timespec interv;
                diff(&startTime, &endTime, &interv);
                cout<<"Total timeuse: "<<double(interv.tv_sec * NANOSECOND + interv.tv_nsec)/NANOSECOND<<"s"<<endl;

                delete[] mch;
                delete[] fakeresult;
                delete[] buffer.Binarystring;

                if (strlen(realresult) == 0) {
                    printf("%s\n", "Error can not be fixed! This program will be shut down!");
                    //CRC未出错的话一定出现了hidden error
                    return 0;
                }

#if DEBUG
                printf("RealresultBit: %s\n", realresult);
#endif

#if DEBUG
                if (pass_crc > 1){ //需更改if(flag == 1)判断条件为flag==2及以上数字，否则永远不会出现假阳性

                printf("%s\n", "Falsepositive happens");
            }
#endif


                /* -------------------------------------------------------------------------- */
                /* --- STAGE : 二进制字符串转十六进制字符串 ---------------------- */

                buffer.Hexstring = new char[BUF_SIZE]; //char类型的PHYPayload
                memset(buffer.Hexstring, 0, BUF_SIZE * sizeof(char));

                buffer.setHexstring(realresult);
                delete[] realresult;
#if DEBUG
                printf("RealresultHex: %s\n", buffer.Hexstring);
#endif

                /* -------------------------------------------------------------------------- */
                /* --- STAGE : Encoding ---------------------- */

                buffer.Hexstring_uint8 = new uint8_t[BUF_SIZE];
                memset(buffer.Hexstring_uint8, 0, BUF_SIZE * sizeof(uint8_t));

                buffer.setHexstring_uint();
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

#if DEBUG
                uint16_t    payload_crc16_calc;
            payload_crc16_calc = sx1302_lora_payload_crc(buffer.Hexstring_uint8, size);
            printf("FixedPayload CRC (0x%04X)\n", payload_crc16_calc);
#endif

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



                /* -------------------------------------------------------------------------- */
                /* --- STAGE : 将Upstream JSON data structure的"data" field里面的数据使用修改后的data_up覆盖 ---------------------- */

                strncpy(buffer_array[index].inter + FindFirstSubchar(buffer_array[index].inter, "data") + 6, data_up, strlen(data_up)); //https://blog.csdn.net/zmhawk/article/details/44600075

#if DEBUG

                //两个库都无法做到这一点，只能手动写函数 (https://github.com/DaveGamble/cJSON/issues/582)

                JSON_Value* root_val = NULL;
                JSON_Object* first_obj = NULL;
                JSON_Array* rxpk_array = NULL;

                root_val = json_parse_string_with_comments((const char*)(buffer_array[index].uint + buffer_array->buff_index));
                rxpk_array = json_object_get_array(json_value_get_object(root_val), "rxpk");
                first_obj = json_array_get_object(rxpk_array, 0);
                json_object_set_string(first_obj, "data", data_up);
                buffer_array[index].inter = json_serialize_to_string(root_val);
                puts(buffer_array[index].inter);

                cJSON* json = NULL;
                cJSON* arrayItem = NULL;
                cJSON* object = NULL;
                cJSON* item = NULL;

                json = cJSON_Parse((const char*)(buffer_array[index].uint + buffer_array->buff_index));
                arrayItem = cJSON_GetObjectItem(json, "rxpk");
                object = cJSON_GetArrayItem(arrayItem, 0);
                item = cJSON_GetObjectItem(object, "data");
                printf("data: %s\n", item->valuestring);
                cJSON_SetValuestring(item, data_up);
                buffer_array[index].inter = cJSON_Print(json);
                puts(buffer_array[index].inter);

#endif

                /* -------------------------------------------------------------------------- */
                /* --- STAGE : 更改stat从-1到1 ---------------------- */

                deleteChar(buffer_array[index].inter, FindFirstSubchar(buffer_array[index].inter, "stat") + 5);
                buffer_array[index].index--;

                /* -------------------------------------------------------------------------- */
                /* --- STAGE : 构造出前12-byte header缺陷的buffer_inter_uint_char ---------------------- */

                buffer.setInter(buffer_array[index].inter); //将bufferi_inter赋值buffer_inter给以后续处理
                buffer.setInter_Uint();
                buffer.setInter_Uint_Char(buffer_array[index].index);

                /* -------------------------------------------------------------------------- */
                /* --- STAGE : 将buff_i的前12-byte(必然不会被修改的header部分) 与buffer_inter_uint_char的第12 byte开始的部分(修改后的Upstream JSON data structure) 组合起来，转换为uint8_t的buffer_send ---------------------- */


                buffer.setSend_First_Part_Char(buffer_array[index].data);
                buffer.setSend_Last_Part_Char();
                buffer.setSend();

                printf("buffer.send: ");
                for (int count = 0; count < buffer_array[index].index; count++) {
                    printf("%02X", buffer.send[count]);
                }
                printf("\n\n");

                delete[] data_up;
                delete[] buffer.inter;
                delete[] buffer.inter_uint_char;

            }
            else {

                //TODO: 两个包CRC不同，说明不是同一个数据包的副本，无法改错
                printf("Both two packets do not have the same FCS, no operation will be taken\n");

                for(int i=0; i<=buffer_num-1; i++){
                    cout<<"buffer_send"<<i+1<<": ";
                    for (int count = 0; count < buffer_array[i].index; count++) {
                        printf("%02X", buffer_array[i].inter_uint[count]);
                    }
                    printf("\n\n");
                }

            }

        }
        else {

            printf("At least one packet is crc correct, no operation will be taken\n\n");

            for(int i=0; i<=buffer_num-1; i++){
                cout<<"buffer_send"<<i+1<<": ";
                for (int count = 0; count < buffer_array[i].index; count++) {
                    printf("%02X", buffer_array[i].inter_uint[count]);
                }
                printf("\n\n");
            }

        }

    }else if(strcmp(choice, "APC")==0){
        /* -------------------------------------------------------------------------- */
        /* --- STAGE : 模拟server_side接收到的buffer数据进行试验---------------------- */
        //TODO: 取多个真实值实验

        int buffer_num = 4;
        Buffer buffer_array[buffer_num];


        BufferSend buffer{};

        for(int i=0; i<=buffer_num-1; i++){
            buffer_array[i].data = new char[BUF_SIZE];
            memset(buffer_array[i].data, 0, BUF_SIZE * sizeof(char));
        }

        buffer_array[0].setData(buff_up_char1);
        buffer_array[1].setData(buff_up_char2);
        buffer_array[2].setData(buff_up_char3);
        buffer_array[3].setData(buff_up_char4);

        for(int i=0; i<=buffer_num-1; i++){
            buffer_array[i].setIndex();
            buffer_array[i].uint[BUF_SIZE] = {0};
            buffer_array[i].setUint();
        }

        /* -------------------------------------------------------------------------- */
        /* --- STAGE : 对中间数据buffer_inter纠错---------------------- */

        for(int i=0; i<=buffer_num-1; i++){
            buffer_array[i].setInter(); //接收到的Upstream JSON data structure
            cout<<"buffer"<<i+1<<".inter: "<<buffer_array[i].inter<<endl;
            buffer_array[i].setInter_Uint();
        }

        /* -------------------------------------------------------------------------- */
        /* --- STAGE : 找到上行数据中需要的属性的值 ---------------------- */
        //TODO: 解决多数据包同时上行情况 (重复数据包接收是因为距离太近)
        //https://forum.rakwireless.com/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/3980/3?u=haowong
        //https://forum.chirpstack.io/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/10886/2?u=shirou_emiya

        Rxpk rxpk_array[buffer_num];


        for(int i=0; i<=buffer_num-1; i++){
            rxpk_array[i].setTime(buffer_array[i].uint,buffer_array->buff_index);
            rxpk_array[i].setStat(buffer_array[i].uint,buffer_array->buff_index);
            rxpk_array[i].setCrc_get(buffer_array[i].uint,buffer_array->buff_index);
            rxpk_array[i].setStr(buffer_array[i].uint,buffer_array->buff_index);
            rxpk_array[i].setRssi(buffer_array[i].uint,buffer_array->buff_index);
        }

        unsigned int crc_get = 0;

#if DEBUG
        printf("rxpk1.stat: %d\n", rxpk_array[0].stat);
    printf("rxpk1.crc_get: %d\n", rxpk_array[0].crc_get);
    printf("rxpk1.str: %s\n", rxpk_array[0].str);
    printf("rssi1: %d\n", rxpk_array[0].rssi);
    printf("time1: %s\n", rxpk_array[0].time);
#endif

        /* -------------------------------------------------------------------------- */
        /* --- STAGE : 当全部上行数据都错且crc值相同时进行纠错 ---------------------- */


        if (compareStat(rxpk_array, buffer_num)) {


            printf("All packets are crc incorrect\n");

            if (compareCRC(rxpk_array, buffer_num)) {

                printf("All packets have the same FCS\n\n");

                printf("Error correction begins\n\n");

                crc_get = rxpk_array[0].crc_get;

                /* -------------------------------------------------------------------------- */
                /* --- STAGE : Decoding ---------------------- */

                for(int i=0; i<=buffer_num-1; i++){
                    buffer_array[i].payload[BUF_SIZE] = {0};
                    buffer_array[i].setSize(rxpk_array[i].str); //与net_downlink相似，都是接收到data，故都用b64_to_bin
                    cout<<"copy"<<i+1<<" of data: "<<rxpk_array[i].str<<endl;
                    delete[] rxpk_array[i].str;
                }

                uint16_t size;

                if ((buffer_array[0].size == buffer_array[1].size)&&(buffer_array[0].size == buffer_array[2].size)&&(buffer_array[0].size == buffer_array[3].size)) {
                    size = buffer_array[0].size;
                }
                else {
                    printf("Error: Not all copies has the same length!\n");
                    return 0;
                }

                /* -------------------------------------------------------------------------- */
                /* --- STAGE : uint8_t转char ---------------------- */ //https://bbs.csdn.net/topics/390141308

                for(int i=0; i<=buffer_num-1; i++){
                    buffer_array[i].Hexstring = new char[BUF_SIZE];
                    memset(buffer_array[i].Hexstring, 0, BUF_SIZE * sizeof(char));

                    buffer_array[i].setHexstring();
                }


#if DEBUG
                printf("M's: %s\n", buffer_array[0].Hexstring);
                printf("M'r: %s\n", buffer_array[1].Hexstring);
#endif



                /* -------------------------------------------------------------------------- */
                /* --- STAGE : 十六进制字符串转二进制字符串 ---------------------- */ //https://blog.csdn.net/weixin_30279751/article/details/95437814

                for(int i=0; i<=buffer_num-1; i++){
                    buffer_array[i].Binarystring = new char[BUF_SIZE];
                    memset(buffer_array[i].Binarystring, 0, BUF_SIZE * sizeof(char));

                    buffer_array[i].setBinarystring();
                    delete[] buffer_array[i].Hexstring;
                }

                /* -------------------------------------------------------------------------- */
                /* --- STAGE : APC ---------------------- */

                buffer.Binarystring = new char[BUF_SIZE]; //Merged error mask / Ambiguity vectors / Va
                memset(buffer.Binarystring, 0, BUF_SIZE * sizeof(char));
                buffer.Binarystring2 = new char[BUF_SIZE]; //APC candidate
                memset(buffer.Binarystring2, 0, BUF_SIZE * sizeof(char));

                LeastReliableMask(buffer_array[0].Binarystring, buffer_array[1].Binarystring, buffer_array[2].Binarystring, buffer_array[3].Binarystring, buffer.Binarystring);
                majorityVoting(buffer_array[0].Binarystring, buffer_array[1].Binarystring, buffer_array[2].Binarystring, buffer_array[3].Binarystring, buffer.Binarystring2);

                /* -------------------------------------------------------------------------- */
                /* --- STAGE : GetCandidate ---------------------- */
                /* -------------------------------------------------------------------------- */
                /* --- STAGE : CRC ---------------------- */


                int Hamming_weight_now = 0;
                for(int loopcount = 0; loopcount<= strlen(buffer.Binarystring)-1; loopcount++){
                    if(buffer.Binarystring[loopcount] == '1'){
                        Hamming_weight_now++;
                    }
                }

#if DEBUG
                printf("Hamming_weight_now: %d\n", Hamming_weight_now);
#endif

                char* mch = new char[BUF_SIZE];
                memset(mch, 0, BUF_SIZE * sizeof(char));

                int index = compareRSSI(rxpk_array, buffer_num);//Selection Combining (SC)
                strcpy(mch, buffer_array[index].Binarystring);

                for(int loopcount=0; loopcount<=buffer_num-1; loopcount++){
                    delete[] buffer_array[loopcount].Binarystring;
                }

#if DEBUG
                printf("MCH: %s\n", mch);
#endif
                char* crc = new char[BUF_SIZE];
                memset(crc, 0, BUF_SIZE * sizeof(char));
                sprintf(crc, "0x%04X", crc_get);
                printf("Processed CRC: %s\n", crc);
                int crc_int = 0;
                sscanf(crc, "%X", &crc_int); //用sscanf而不是atoi的原因是虽然linux有atoi，但是crc最前面的0还是没了
                delete[] crc;
#if DEBUG
                printf("CRC int: %x\n", crc_int);
#endif
                int i = 0;
#if DEBUG
                printf("Mask: %s\n", s);
#endif

                int Hamming_weight_max = 30; //预设的最多纠错比特位数量
                if (Hamming_weight_now > Hamming_weight_max) {

                    printf("%s: %d\n", "Hamming weight is larger than the max number", Hamming_weight_max);
                    printf("This program will be shut down!\n");
                    return 0;

                } {
#if DEBUG
                    printf("Hamming Weight: %d\n", Hamming_weight_now);
#endif
                }

                char* fakeresult = new char[BUF_SIZE]; //每次candidate与mch异或的中间产值
                memset(fakeresult, 0, BUF_SIZE * sizeof(char));

                char* realresult = new char[BUF_SIZE]; //符合CRC校验的fakeresult
                memset(realresult, 0, BUF_SIZE * sizeof(char));
                int total_number = 0; //一共运行的次数
                int pass_crc = 0; //符合CRC校验的次数

                compareCRC(crc_int, buffer.Binarystring2, realresult, size, pass_crc);
                if(strlen(realresult)){
                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 二进制字符串转十六进制字符串 ---------------------- */

                    buffer.Hexstring = new char[BUF_SIZE]; //char类型的PHYPayload
                    memset(buffer.Hexstring, 0, BUF_SIZE * sizeof(char));

                    buffer.setHexstring(realresult);
                    delete[] realresult;
#if DEBUG
                    printf("RealresultHex: %s\n", buffer.Hexstring);
#endif

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : Encoding ---------------------- */

                    buffer.Hexstring_uint8 = new uint8_t[BUF_SIZE];
                    memset(buffer.Hexstring_uint8, 0, BUF_SIZE * sizeof(uint8_t));

                    buffer.setHexstring_uint();
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

#if DEBUG
                    uint16_t    payload_crc16_calc;
            payload_crc16_calc = sx1302_lora_payload_crc(buffer.Hexstring_uint8, size);
            printf("FixedPayload CRC (0x%04X)\n", payload_crc16_calc);
#endif

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



                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 将Upstream JSON data structure的"data" field里面的数据使用修改后的data_up覆盖 ---------------------- */

                    strncpy(buffer_array[index].inter + FindFirstSubchar(buffer_array[index].inter, "data") + 6, data_up, strlen(data_up)); //https://blog.csdn.net/zmhawk/article/details/44600075

#if DEBUG
                    //TODO: JSON serialization

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
#endif

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 更改stat从-1到1 ---------------------- */

                    deleteChar(buffer_array[index].inter, FindFirstSubchar(buffer_array[index].inter, "stat") + 5);
                    buffer_array[index].index--;

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 构造出前12-byte header缺陷的buffer_inter_uint_char ---------------------- */

                    buffer.setInter(buffer_array[index].inter); //将bufferi_inter赋值buffer_inter给以后续处理
                    buffer.setInter_Uint();
                    buffer.setInter_Uint_Char(buffer_array[index].index);

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 将buff_i的前12-byte(必然不会被修改的header部分) 与buffer_inter_uint_char的第12 byte开始的部分(修改后的Upstream JSON data structure) 组合起来，转换为uint8_t的buffer_send ---------------------- */


                    buffer.setSend_First_Part_Char(buffer_array[index].data);
                    buffer.setSend_Last_Part_Char();
                    buffer.setSend();

                    printf("buffer.send: ");
                    for (int count = 0; count < buffer_array[index].index; count++) {
                        printf("%02X", buffer.send[count]);
                    }
                    printf("\n\n");

                    delete[] data_up;
                    delete[] buffer.inter;
                    delete[] buffer.inter_uint_char;

                }else{
                    printf("%s\n", "Error can not be fixed! APC continues!");

                    struct timespec startTime;
                    clock_gettime(CLOCK_REALTIME, &startTime);

                    if(Hamming_weight_now <= Hamming_weight_max/2){
                        incremental_correct(buffer.Binarystring, mch, Hamming_weight_now, crc_int, fakeresult, realresult, size, pass_crc, total_number, startTime);
                    }else{
                        correct(buffer.Binarystring, mch, Hamming_weight_now, crc_int, fakeresult, realresult, size, pass_crc, total_number, startTime);
                    }

                    struct timespec endTime;
                    clock_gettime(CLOCK_REALTIME, &endTime);

                    struct timespec interv;
                    diff(&startTime, &endTime, &interv);
                    cout<<"Total timeuse: "<<double(interv.tv_sec * NANOSECOND + interv.tv_nsec)/NANOSECOND<<"s"<<endl;

                    delete[] mch;
                    delete[] fakeresult;
                    delete[] buffer.Binarystring;

                    if (strlen(realresult) == 0) {
                        printf("%s\n", "Error can not be fixed! This program will be shut down!");
                        //CRC未出错的话一定出现了hidden error
                        return 0;
                    }

#if DEBUG
                    printf("RealresultBit: %s\n", realresult);
#endif

#if DEBUG
                    if (pass_crc > 1){ //需更改if(flag == 1)判断条件为flag==2及以上数字，否则永远不会出现假阳性

                printf("%s\n", "Falsepositive happens");
            }
#endif

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 二进制字符串转十六进制字符串 ---------------------- */

                    buffer.Hexstring = new char[BUF_SIZE]; //char类型的PHYPayload
                    memset(buffer.Hexstring, 0, BUF_SIZE * sizeof(char));

                    buffer.setHexstring(realresult);
                    delete[] realresult;
#if DEBUG
                    printf("RealresultHex: %s\n", buffer.Hexstring);
#endif

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : Encoding ---------------------- */

                    buffer.Hexstring_uint8 = new uint8_t[BUF_SIZE];
                    memset(buffer.Hexstring_uint8, 0, BUF_SIZE * sizeof(uint8_t));

                    buffer.setHexstring_uint();
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

#if DEBUG
                    uint16_t    payload_crc16_calc;
            payload_crc16_calc = sx1302_lora_payload_crc(buffer.Hexstring_uint8, size);
            printf("FixedPayload CRC (0x%04X)\n", payload_crc16_calc);
#endif

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



                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 将Upstream JSON data structure的"data" field里面的数据使用修改后的data_up覆盖 ---------------------- */

                    strncpy(buffer_array[index].inter + FindFirstSubchar(buffer_array[index].inter, "data") + 6, data_up, strlen(data_up)); //https://blog.csdn.net/zmhawk/article/details/44600075

#if DEBUG
                    //TODO: JSON serialization

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
#endif

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 更改stat从-1到1 ---------------------- */

                    deleteChar(buffer_array[index].inter, FindFirstSubchar(buffer_array[index].inter, "stat") + 5);
                    buffer_array[index].index--;

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 构造出前12-byte header缺陷的buffer_inter_uint_char ---------------------- */

                    buffer.setInter(buffer_array[index].inter); //将bufferi_inter赋值buffer_inter给以后续处理
                    buffer.setInter_Uint();
                    buffer.setInter_Uint_Char(buffer_array[index].index);

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 将buff_i的前12-byte(必然不会被修改的header部分) 与buffer_inter_uint_char的第12 byte开始的部分(修改后的Upstream JSON data structure) 组合起来，转换为uint8_t的buffer_send ---------------------- */


                    buffer.setSend_First_Part_Char(buffer_array[index].data);
                    buffer.setSend_Last_Part_Char();
                    buffer.setSend();

                    printf("buffer.send: ");
                    for (int count = 0; count < buffer_array[index].index; count++) {
                        printf("%02X", buffer.send[count]);
                    }
                    printf("\n\n");

                    delete[] data_up;
                    delete[] buffer.inter;
                    delete[] buffer.inter_uint_char;

                }

            }
            else {

                printf("Not all packets have the same FCS, no operation will be taken\n");

                for(int i=0; i<=buffer_num-1; i++){
                    cout<<"buffer_send"<<i+1<<": ";
                    for (int count = 0; count < buffer_array[i].index; count++) {
                        printf("%02X", buffer_array[i].inter_uint[count]);
                    }
                    printf("\n\n");
                }

            }

        }
        else {

            printf("At least one packet is crc correct, no operation will be taken\n\n");

            for(int i=0; i<=buffer_num-1; i++){
                cout<<"buffer_send"<<i+1<<": ";
                for (int count = 0; count < buffer_array[i].index; count++) {
                    printf("%02X", buffer_array[i].inter_uint[count]);
                }
                printf("\n\n");
            }

        }
    }


    return 0;
}