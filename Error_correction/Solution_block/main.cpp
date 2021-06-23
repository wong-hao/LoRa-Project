#include <functional>

#include"Component_1_fakesend/inc/header_1_1.h"
#include"Component_1_getCandidate/inc/header_1_2.h"
#include"Component_1_getMast/inc/header_1_3.h"

#include "Component_2_server_side/inc/header_2.h"
#include "tools/inc/payload_crc.h"
#include "tools/inc/parson.h"

#include"tools/inc/payload_diff.h"

#include"Component_3_connection_error_correction/inc/header_3.h"

#include "tools/inc/base64.h"


extern char s[BUF_SIZE], d[BUF_SIZE];
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

    Buffer buffer1{};
    Buffer buffer2{};
    Buffer buffer{};

    buffer1.data = new char[BUF_SIZE];
    memset(buffer1.data, 0, BUF_SIZE * sizeof(char));
    buffer2.data = new char[BUF_SIZE];
    memset(buffer2.data, 0, BUF_SIZE * sizeof(char));

    while (1) {

        int clnt_sock1 = accept(serv_sock1, (struct sockaddr*)&clnt_addr1, &clnt_addr_size1);
        //TODO: 解决掉由于只有一个gateway成功连接导致的accept阻塞问题
        int clnt_sock2 = accept(serv_sock2, (struct sockaddr*)&clnt_addr2, &clnt_addr_size2);

        //读取client传回的数据
        recv(clnt_sock1, buffer1.data, BUF_SIZE, 0);
        //recv(clnt_sock1, buffer1.data, sizeof(buffer1.data) - 1, 0);
        recv(clnt_sock2, buffer2.data, BUF_SIZE, 0);

        /*测试代码
        printf("buffer1: %s\n", buffer1.data);
        printf("\n");
        printf("buffer2: %s\n", buffer2.data);
        printf("\n");
        */

        buffer1.index = strlen(buffer1.data) / 2;
        buffer2.index = strlen(buffer2.data) / 2;

        buffer1.uint[BUF_SIZE] = {0};
        buffer2.uint[BUF_SIZE] = {0};

        Char2Uint(buffer1.data, buffer1.uint);
        Char2Uint(buffer2.data, buffer2.uint);


        /* -------------------------------------------------------------------------- */
        /* --- STAGE : 对中间数据buffer_inter纠错---------------------- */
        //TODO: false and true带来的多个包同时转发


        buffer1.inter = (char*)(buffer1.uint + buff_index); //接收到的Upstream JSON data structure
        buffer2.inter = (char*)(buffer2.uint + buff_index);
        //printf("buffer1.inter: %s\n", buffer1.inter);
        //printf("\n");
        //printf("buffer2.inter: %s\n", buffer2.inter);
        //printf("\n");

        buffer1.inter_uint = (uint8_t*)(buffer1.inter - buff_index);
        buffer2.inter_uint = (uint8_t*)(buffer2.inter - buff_index);

        /* -------------------------------------------------------------------------- */
        /* --- STAGE : 找到上行数据中需要的属性的值 ---------------------- */
        //TODO: 解决多数据包同时上行情况 (重复数据包接收是因为距离太近)
        //https://forum.rakwireless.com/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/3980/3?u=haowong
        //https://forum.chirpstack.io/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/10886/2?u=shirou_emiya

        Rxpk rxpk1{};
        Rxpk rxpk2{};

        rxpk1.stat = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer1.uint + buff_index))), "rxpk"), 0), "stat"));
        rxpk2.stat = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer2.uint + buff_index))), "rxpk"), 0), "stat"));


        rxpk1.crc_get = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer1.uint + buff_index))), "rxpk"), 0), "crc"));
        rxpk2.crc_get = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer2.uint + buff_index))), "rxpk"), 0), "crc"));
        unsigned int crc_get = 0;

        rxpk1.str = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer1.uint + buff_index))), "rxpk"), 0), "data");
        rxpk2.str = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer2.uint + buff_index))), "rxpk"), 0), "data");

        rxpk1.rssi = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer1.uint + buff_index))), "rxpk"), 0), "rssi"));
        rxpk2.rssi = (int)json_value_get_number(json_object_get_value(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer2.uint + buff_index))), "rxpk"), 0), "rssi"));


        rxpk1.time = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer1.uint + buff_index))), "rxpk"), 0), "time");
        rxpk2.time = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer2.uint + buff_index))), "rxpk"), 0), "time");

        /*测试代码
        printf("rxpk1.stat: %d\n", rxpk1.stat);
        printf("rxpk1.crc_get: %d\n", rxpk1.crc_get);
        printf("rxpk1.str: %s\n", rxpk1.str);
        printf("rxpk1.rssi: %d\n", rxpk1.rssi);
        printf("rxpk1.time: %s\n", rxpk1.time);
        */


        /* -------------------------------------------------------------------------- */
        /* --- STAGE : 当两个上行数据都错且crc值相同时进行纠错 ---------------------- */


        if ((rxpk1.stat == -1) && (rxpk2.stat == -1)) {



            printf("Both two packets are crc incorrect\n");

            if (rxpk1.crc_get == rxpk2.crc_get) {

                printf("Both two packets have the same FCS\n\n");

                printf("Error correction begins\n\n");

                crc_get = rxpk1.crc_get;

                /* -------------------------------------------------------------------------- */
                /* --- STAGE : Decoding ---------------------- */

                buffer1.payload[BUF_SIZE] = {0};

                buffer1.size = b64_to_bin(rxpk1.str, strlen(rxpk1.str), buffer1.payload, sizeof buffer1.payload); //与net_downlink相似，都是接收到data，故都用b64_to_bin
                /*测试代码
                printf("Copy_1 of data: %s\n", rxpk1.str);
                */
                delete[] rxpk1.str;

                buffer2.payload[BUF_SIZE] = {0};

                buffer2.size = b64_to_bin(rxpk2.str, strlen(rxpk2.str), buffer2.payload, sizeof buffer2.payload); //与net_downlink相似，都是接收到data，故都用b64_to_bin
                /*测试代码
                printf("Copy_2 of data: %s\n", rxpk2.str);
                */
                delete[] rxpk2.str;


                uint16_t size;

                if (buffer1.size == buffer2.size) {
                    size = buffer1.size;
                }
                else {
                    printf("Error: length1 is not equal to length2. This program will be shut down!"); //TODO: 一个单包一个多包接收必然会出现这种情况
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
                    printf("函数出错！\n"); //TODO: 一个单包一个多包接收必然会出现这种情况
                    return 1;
                }

                /* -------------------------------------------------------------------------- */
                /* --- STAGE : GetCandidate ---------------------- */
                /* -------------------------------------------------------------------------- */
                /* --- STAGE : CRC ---------------------- */


                char* mch = new char[BUF_SIZE];
                memset(mch, 0, BUF_SIZE * sizeof(char));
                if (rxpk1.rssi >= rxpk2.rssi) {

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
                strcpy(s, buffer.Binarystring);
                delete[] buffer.Binarystring;
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
                /*测试代码
                printf("Corrected data: %s\n", data_up);
                */
                delete[] data_up_uint8;

                /* 测试代码
                uint16_t    payload_crc16_calc;
                payload_crc16_calc = sx1302_lora_payload_crc(buffer.Hexstring_uint8, size);
                printf("FixedPayload CRC (0x%04X)\n", payload_crc16_calc);
                */


                /* -------------------------------------------------------------------------- */
                /* --- STAGE : 修改Upstream JSON data structure ---------------------- */
                //TODO: 解决多数据包同时上行情况

                JSON_Value* root_val = NULL;
                JSON_Object* first_obj = NULL;
                JSON_Array* rxpk_array = NULL;

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
                    root_val = json_parse_string_with_comments((const char*)(buffer1.uint + buff_index));
                    rxpk_array = json_object_get_array(json_value_get_object(root_val), "rxpk");
                    first_obj = json_array_get_object(rxpk_array, 0);
                    json_object_set_string(first_obj, "data", data_up);
                    buffer1.inter = json_serialize_to_string(root_val);
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

                    /*测试代码
                    printf("buffer.send: ");
                    for (int count = 0; count < buffer1.index; count++) {
                        printf("%02X", buffer.send[count]);
                    }
                    printf("\n\n");
                    */


                    delete[] data_up;
                    delete[] buffer.inter;
                    delete[] buffer.inter_uint_char;


                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 发送---------------------- */


                    send(sock_up, (void*)buffer.send, buffer1.index, 0);

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

                    /*测试代码
                    printf("buffer.send: ");
                    for (int count = 0; count < buffer2.index; count++) {
                        printf("%02X", buffer.send[count]);
                    }
                     printf("\n\n");
                   */


                    delete[] data_up;
                    delete[] buffer.inter;
                    delete[] buffer.inter_uint_char;


                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 发送---------------------- */


                    send(sock_up, (void*)buffer.send, buffer2.index, 0);


                }

            }
            else {

                printf("Both two packets do not have the same FCS, no operation will be taken\n");

                /*测试代码
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
                */


                /* -------------------------------------------------------------------------- */
                /* --- STAGE : 发送---------------------- */


                send(sock_up, (void*)buffer1.inter_uint, buffer1.index, 0);
                send(sock_up, (void*)buffer2.inter_uint, buffer2.index, 0);


            }

        }
        else {

            printf("At least one packet is crc correct, no operation will be taken\n\n");

            /*测试代码
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
            */


            /* -------------------------------------------------------------------------- */
            /* --- STAGE : 发送---------------------- */


            send(sock_up, (void*)buffer1.inter_uint, buffer1.index, 0);
            send(sock_up, (void*)buffer2.inter_uint, buffer2.index, 0);


        }



        //关闭套接字
        close(clnt_sock1);
        close(clnt_sock2);
        memset(buffer1.data, 0, BUF_SIZE);  //重置缓冲区
        memset(buffer2.data, 0, BUF_SIZE);  //重置缓冲区

    }

    delete[] buffer1.data;
    delete[] buffer2.data;
    close(serv_sock1);
    close(serv_sock2);
    close(sock_up);

    return 0;

}

