#include <functional>

#include "header_1_1.h"
#include "header_1_2.h"
#include "header_1_3.h"
#include "payload_crc.h"
#include "payload_diff.h"

#include "header_3.h"

#include "base64.h"

#include "parson.h"


extern char s[BUF_SIZE], d[BUF_SIZE];



int main() {

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 建立接收socket1 ---------------------- */

    //创建套接字
    int serv_sock1 = socket(AF_INET, SOCK_STREAM, 0);

    //将套接字和IP、端口绑定
    struct sockaddr_in serv_addr_receive1;
    memset(&serv_addr_receive1, 0, sizeof(serv_addr_receive1));  //每个字节都用0填充
    serv_addr_receive1.sin_family = AF_INET;  //使用IPv4地址
    serv_addr_receive1.sin_addr.s_addr = INADDR_ANY;
    serv_addr_receive1.sin_port = htons(1680);  //端口
    bind(serv_sock1, (struct sockaddr*)&serv_addr_receive1, sizeof(serv_addr_receive1));


    //进入监听状态，等待用户发起请求
    listen(serv_sock1, 20);

    //接收客户端请求
    struct sockaddr_in clnt_addr1;
    socklen_t clnt_addr_size1 = sizeof(clnt_addr1);

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 建立接收socket2 ---------------------- */

    //创建套接字
    int serv_sock2 = socket(AF_INET, SOCK_STREAM, 0);

    //将套接字和IP、端口绑定
    struct sockaddr_in serv_addr_receive2;
    memset(&serv_addr_receive2, 0, sizeof(serv_addr_receive2));  //每个字节都用0填充
    serv_addr_receive2.sin_family = AF_INET;  //使用IPv4地址
    serv_addr_receive2.sin_addr.s_addr = INADDR_ANY;
    serv_addr_receive2.sin_port = htons(1690); //端口
    bind(serv_sock2, (struct sockaddr*)&serv_addr_receive2, sizeof(serv_addr_receive2));


    //进入监听状态，等待用户发起请求
    listen(serv_sock2, 20);


    //接收客户端请求
    struct sockaddr_in clnt_addr2;
    socklen_t clnt_addr_size2 = sizeof(clnt_addr2);

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 建立发射socket ---------------------- */

    /* network socket creation */ //socket套接字网络通信
    struct addrinfo hints;
    struct addrinfo* result; /* store result of getaddrinfo */
    struct addrinfo* q; /* pointer to move into *result data */
    char host_name[64];
    char port_name[64];

    int i; /* loop variable and temporary variable for return value */


/* prepare hints to open network sockets */ //既为upstream也为downstream打基础
    memset(&hints, 0, sizeof hints); //hints
    hints.ai_family = AF_INET; //IPV4 /* WA: Forcing IPv4 as AF_UNSPEC(IPV4 and IPV6) makes connection on localhost to fail */
    hints.ai_socktype = SOCK_DGRAM; //UDP
    //hints.ai_protocol取默认值0，系统会自动推演出应该使用UDP协议

    /* look for server address w/ upstream port */
    i = getaddrinfo(serv_addr, serv_port_up, &hints, &result);
    //serv_addr、serv_port_up由parse_gateway_configuration得出；从hints读取信息存储到result
    //因为IP + port -> socket
    if (i != 0) {
        printf("ERROR: [up] getaddrinfo on address %s (PORT %s) returned %s\n", serv_addr, serv_port_up, gai_strerror(i));
        exit(EXIT_FAILURE);
    }

    /* try to open socket for upstream traffic */
    for (q = result; q != NULL; q = q->ai_next) { //q指向result，q的属性都是上面getaddrinfo得到的；因为一个域名可能不止一个IP地址，所以，需要遍历res中的next，如下，是否还有下一个节点
        sock_up = socket(q->ai_family, q->ai_socktype, q->ai_protocol); //创建套接字sock_up
        if (sock_up == -1) continue; /* try next field */
        else break; /* success, get out of loop */ //得到sock_up后跳出for循环，没有必要循环到结束条件q==NULL
    }
    if (q == NULL) { //一直循环到了结束条件q==NULL都没获得sock_up
        printf("ERROR: [up] failed to open socket to any of server %s addresses (port %s)\n", serv_addr, serv_port_up);
        i = 1;
        for (q = result; q != NULL; q = q->ai_next) {
            getnameinfo(q->ai_addr, q->ai_addrlen, host_name, sizeof host_name, port_name, sizeof port_name, NI_NUMERICHOST);
            //与getaddrinfo两级反转: socket -> IP + port
            printf("INFO: [up] result %i host:%s service:%s\n", i, host_name, port_name);
            ++i;
        }
        exit(EXIT_FAILURE); //异常退出程序
    }

    /* connect so we can send/receive packet with the server only */
    i = connect(sock_up, q->ai_addr, q->ai_addrlen); //连接上行socket；q为for循环break出时的值
    if (i != 0) {
        printf("ERROR: [up] connect returned %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(result); //释放掉好进行下行通信

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 开始处理数据 ---------------------- */



    char* buffer1 = new char[BUF_SIZE];
    char* buffer2 = new char[BUF_SIZE];
    memset(buffer1, 0, BUF_SIZE * sizeof(char));
    memset(buffer2, 0, BUF_SIZE * sizeof(char));


    while (1) {

        int clnt_sock1 = accept(serv_sock1, (struct sockaddr*)&clnt_addr1, &clnt_addr_size1);
        //TODO: 解决掉由于只有一个gateway成功连接导致的accept阻塞问题
        int clnt_sock2 = accept(serv_sock2, (struct sockaddr*)&clnt_addr2, &clnt_addr_size2);

        //读取client传回的数据
        recv(clnt_sock1, buffer1, BUF_SIZE, 0);
        //recv(clnt_sock1, buffer1, sizeof(buffer1) - 1, 0);
        recv(clnt_sock2, buffer2, BUF_SIZE, 0);

        /*测试代码
        printf("buffer1: %s\n", buffer1);
        printf("\n");
        printf("buffer2: %s\n", buffer2);
        printf("\n");
        */

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
        //TODO: false and true带来的多个包同时转发


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


        int stat1 = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint1 + 12))), "rxpk"), 0), "stat"));
        int stat2 = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint2 + 12))), "rxpk"), 0), "stat"));


        int crc_get1 = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint1 + 12))), "rxpk"), 0), "crc"));
        int crc_get2 = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint2 + 12))), "rxpk"), 0), "crc"));
        unsigned int crc_get = 0;

        const char* str1 = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint1 + 12))), "rxpk"), 0), "data");
        const char* str2 = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint2 + 12))), "rxpk"), 0), "data");

        int rssi1 = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint1 + 12))), "rxpk"), 0), "rssi"));
        int rssi2 = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint2 + 12))), "rxpk"), 0), "rssi"));


        const char* time1 = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint1 + 12))), "rxpk"), 0), "time");
        const char* time2 = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint2 + 12))), "rxpk"), 0), "time");

        /*测试代码
        printf("stat1: %d\n", stat1);
        printf("crc_get1: %d\n", crc_get1);
        printf("str1: %s\n", str1);
        printf("rssis1: %d\n", rssis1);
        printf("time1: %s\n", time1);
        */


        /* -------------------------------------------------------------------------- */
        /* --- STAGE : 当两个上行数据都错且crc值相同时进行纠错 ---------------------- */


        if ((stat1 == -1) && (stat2 == -1)) {



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
                /*测试代码
                printf("Copy_1 of data: %s\n", str1);
                */
                delete[] str1;


                uint8_t  payload2[BUF_SIZE];   /*!> buffer containing the payload */
                uint16_t size2; //json数据包里自带的，但mqtt event没有
                size2 = b64_to_bin(str2, strlen(str2), payload2, sizeof payload2); //与net_downlink相似，都是接收到data，故都用b64_to_bin
                /*测试代码
                printf("Copy_2 of data: %s\n", str2);
                */
                delete[] str2;


                uint16_t size;

                if (size1 == size2) {
                    size = size1;
                }
                else {
                    printf("Error: length1 is not equal to length2. This program will be shut down!"); //TODO: 一个单包一个多包接收必然会出现这种情况
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
                    printf("函数出错！\n"); //TODO: 一个单包一个多包接收必然会出现这种情况
                    return 1;
                }

                /* -------------------------------------------------------------------------- */
                /* --- STAGE : GetCandidate ---------------------- */
                /* -------------------------------------------------------------------------- */
                /* --- STAGE : CRC ---------------------- */


                char* mch = new char[BUF_SIZE];
                memset(mch, 0, BUF_SIZE * sizeof(char));
                if (rssi1 >= rssi2) {

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
                /*测试代码
                printf("Processed CRC: %s\n", crc);
                */
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
                    printf("This program will be shut down!\n"); //TODO: 一个单包一个多包接收必然会出现这种情况
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
                /*测试代码
                printf("Corrected data: %s\n", data_up);
                */
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

                    /*测试代码
                    printf("buffer_send: ");
                    for (int count = 0; count < buff_index1; count++) {
                        printf("%02X", buffer_send[count]);
                    }
                    printf("\n\n");
                    */


                    delete[] data_up;
                    delete[] buffer_inter;
                    delete[] buffer_inter_uint_char;


                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 发送---------------------- */


                    send(sock_up, (void*)buffer_send, buff_index1, 0);

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

                    /*测试代码
                    printf("buffer_send: ");
                    for (int count = 0; count < buff_index2; count++) {
                        printf("%02X", buffer_send[count]);
                    }
                     printf("\n\n");
                   */


                    delete[] data_up;
                    delete[] buffer_inter;
                    delete[] buffer_inter_uint_char;


                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 发送---------------------- */


                    send(sock_up, (void*)buffer_send, buff_index2, 0);


                }

            }
            else {

                printf("Both two packets do not have the same FCS, no operation will be taken\n");

                /*测试代码
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
                */


                /* -------------------------------------------------------------------------- */
                /* --- STAGE : 发送---------------------- */


                send(sock_up, (void*)buffer1_inter_uint, buff_index1, 0);
                send(sock_up, (void*)buffer2_inter_uint, buff_index2, 0);


            }

        }
        else {

            printf("At least one packet is crc correct, no operation will be taken\n\n");

            /*测试代码
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
            */


            /* -------------------------------------------------------------------------- */
            /* --- STAGE : 发送---------------------- */


            send(sock_up, (void*)buffer1_inter_uint, buff_index1, 0);
            send(sock_up, (void*)buffer2_inter_uint, buff_index2, 0);


        }



        //关闭套接字
        close(clnt_sock1);
        close(clnt_sock2);
        memset(buffer1, 0, BUF_SIZE);  //重置缓冲区
        memset(buffer2, 0, BUF_SIZE);  //重置缓冲区

    }

    delete[] buffer1;
    delete[] buffer2;
    close(serv_sock1);
    close(serv_sock2);
    close(sock_up);




    return 0;

}

