//https://www.cnblogs.com/wuyepeng/p/9726771.html

#include"header_1_1.h"
#include"header_1_2.h"
#include"header_1_3.h"
#include "payload_crc.h"
#include"header_1_6.h"
#include "parson.h"

#include"payload_diff.h"
#include"header_2_2.h"

#include"header_3.h"

#include "base64.h"



extern char s[BUF_SIZE], d[BUF_SIZE];


int main() {


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

    int ser_souck_fd;

    char input_message[BUF_SIZE] = {0};
    char* resv_message = new char[BUF_SIZE];

    char* buffer1 = new char[BUF_SIZE];
    memset(buffer1, 0, BUF_SIZE * sizeof(char));
    char* buffer2 = new char[BUF_SIZE];
    memset(buffer2, 0, BUF_SIZE * sizeof(char));

    struct sockaddr_in ser_addr;
    ser_addr.sin_family = AF_INET;    //IPV4
    ser_addr.sin_port = htons(ser_port);
    ser_addr.sin_addr.s_addr = INADDR_ANY;  //指定的是所有地址

    //creat socket
    if ((ser_souck_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("creat failure");
        return -1;
    }

    //bind soucket
    if (bind(ser_souck_fd, (const struct sockaddr*)&ser_addr, sizeof(ser_addr)) < 0)
    {
        perror("bind failure");
        return -1;
    }

    //listen
    if (listen(ser_souck_fd, backlog) < 0)
    {
        perror("listen failure");
        return -1;
    }


    //fd_set
    fd_set ser_fdset;
    int max_fd = 1;
    struct timeval mytime;
    //printf("wait for client connnect!\n");

    while (1)
    {
        mytime.tv_sec = 27;
        mytime.tv_usec = 0;

        FD_ZERO(&ser_fdset);

        //add standard input
        FD_SET(0, &ser_fdset);
        if (max_fd < 0)
        {
            max_fd = 0;
        }

        //add serverce
        FD_SET(ser_souck_fd, &ser_fdset);
        if (max_fd < ser_souck_fd)
        {
            max_fd = ser_souck_fd;
        }

        //add client
        for (int i = 0; i < CLI_NUM; i++)  //用数组定义多个客户端fd
        {
            if (client_fds[i] != 0)
            {
                FD_SET(client_fds[i], &ser_fdset);
                if (max_fd < client_fds[i])
                {
                    max_fd = client_fds[i];
                }
            }
        }

        //select多路复用
        int ret = select(max_fd + 1, &ser_fdset, NULL, NULL, &mytime);

        if (ret < 0)
        {
            perror("select failure\n");
            continue;
        }

        else if (ret == 0)
        {
            printf("time out!");
            continue;
        }

        else
        {
            if (FD_ISSET(0, &ser_fdset)) //标准输入是否存在于ser_fdset集合中（也就是说，检测到输入时，做如下事情）
            {
                printf("send message to");
                memset(input_message, 0, BUF_SIZE * sizeof(char));
                fgets(input_message, BUF_SIZE, stdin);

                for (int i = 0; i < CLI_NUM; i++)
                {
                    if (client_fds[i] != 0)
                    {
                        printf("client_fds[%d]=%d\n", i, client_fds[i]);
                        send(client_fds[i], input_message, BUF_SIZE, 0);
                    }
                }

            }

            if (FD_ISSET(ser_souck_fd, &ser_fdset))
            {
                struct sockaddr_in client_address;
                socklen_t address_len;
                int client_sock_fd = accept(ser_souck_fd, (struct sockaddr*)&client_address, &address_len);
                if (client_sock_fd > 0)
                {
                    int flags = -1;
                    //一个客户端到来分配一个fd，CLI_NUM=3，则最多只能有三个客户端，超过4以后跳出for循环，flags重新被赋值为-1
                    for (int i = 0; i < CLI_NUM; i++)
                    {
                        if (client_fds[i] == 0)
                        {
                            flags = i;
                            client_fds[i] = client_sock_fd;
                            break;
                        }
                    }


                    if (flags >= 0)
                    {
                        //printf("new user client[%d] add sucessfully!\n", flags);

                    }

                    else //flags=-1
                    {
                        char full_message[] = "the client is full!can't join!\n";
                        memset(input_message, 0, BUF_SIZE * sizeof(char));
                        strncpy(input_message, full_message, 100);
                        send(client_sock_fd, input_message, BUF_SIZE, 0);

                    }
                }
            }

        }

        //deal with the message

        for (int i = 0; i < CLI_NUM; i++)
        {
            if (client_fds[i] != 0)
            {
                if (FD_ISSET(client_fds[i], &ser_fdset))
                {
                    memset(resv_message, 0, BUF_SIZE * sizeof(char));
                    int byte_num = read(client_fds[i], resv_message, BUF_SIZE);
                    if (byte_num > 0)
                    {
                        //printf("message form client[%d]:%s\n", i, resv_message);

                        if (resv_message[23] == '6') {
                            strcpy(buffer1, resv_message);
                        }
                        else if (resv_message[23] == '7') {
                            strcpy(buffer2, resv_message);
                        }

                        delete[] resv_message;
                    	
                        //printf("buffer1: %s\n", buffer1);
                        //printf("buffer2: %s\n", buffer2);

                        int buff_index1 = strlen(buffer1) / 2;
                        int buff_index2 = strlen(buffer2) / 2;

                        uint8_t  buffer_uint1[BUF_SIZE] = { 0 };
                        uint8_t  buffer_uint2[BUF_SIZE] = { 0 };
                        Char2Uint(buffer1, buffer_uint1);
                        Char2Uint(buffer2, buffer_uint2);


                        /* -------------------------------------------------------------------------- */
                        /* --- STAGE : 对中间数据buffer_inter纠错---------------------- */
                        //TODO: false and true带来的多个包同时转发


                        char* buffer1_inter = (char*)(buffer_uint1 + 12);
                        char* buffer2_inter = (char*)(buffer_uint2 + 12);
                        //printf("buffer1_inter: %s\n", buffer1_inter);
                        //printf("\n");
                        //printf("buffer2_inter: %s\n", buffer2_inter);
                        //printf("\n");

                        uint8_t* buffer1_inter_uint = (uint8_t*)(buffer1_inter - 12);
                        uint8_t* buffer2_inter_uint = (uint8_t*)(buffer2_inter - 12);

                        /* -------------------------------------------------------------------------- */
						/* --- STAGE : select的异步处理---------------------- */
                    	
                        const char* time1 = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint1 + 12))), "rxpk"), 0), "time");
                        const char* time2 = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint2 + 12))), "rxpk"), 0), "time");

                        if (buff_index1 != 0 && buff_index2 != 0) {

                        	if(strcmp(time1,time2)==0)
                        	{
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

                                /*测试代码
                                printf("stat1: %d\n", stat1);
                                printf("crc_get1: %d\n", crc_get1);
                                printf("str1: %s\n", str1);
                                printf("rssis1: %d\n", rssis1);
                                printf("time1: %s\n", time1);
                                */


                                /* -------------------------------------------------------------------------- */
                                /* --- STAGE : 当两个上行数据都错且crc值相同时进行纠错 ---------------------- */


                                if ((stat1 == -1) && stat2 == -1) {


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

                                        char* data_up = new char[BUF_SIZE]; //char类型的PHYPayload
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

                                        char* buffer_inter = new char[BUF_SIZE];
                                        memset(buffer_inter, 0, BUF_SIZE * sizeof(char));

                                        char* buffer_inter_uint_char = new char[BUF_SIZE];
                                        memset(buffer_inter_uint_char, 0, BUF_SIZE * sizeof(char));

                                        char buffer_send_first_part_char[BUF_SIZE] = { 0 };

                                        char buffer_send_last_part_char[BUF_SIZE] = { 0 };

                                        uint8_t* buffer_send = new uint8_t[BUF_SIZE];
                                        memset(buffer_send, 0, BUF_SIZE * sizeof(uint8_t));

                                        if (rssi1 >= rssi2) {


                                            /* -------------------------------------------------------------------------- */
                                            /* --- STAGE : 构造出前24个字符缺陷的buffer_inter_uint_char：替换data_up ---------------------- */

                                            strncpy(buffer1_inter + FindFirstSubchar(buffer1_inter, report2) + 6, data_up, strlen(data_up)); //https://blog.csdn.net/zmhawk/article/details/44600075

                                            /* -------------------------------------------------------------------------- */
                                            /* --- STAGE : 构造出前24个字符缺陷的buffer_inter_uint_char：更改stat从1到1 ---------------------- */

                                            deleteChar(buffer1_inter, FindFirstSubchar(buffer1_inter, report1) + 5);
                                            buff_index1--;

                                            strcpy(buffer_inter, buffer1_inter);
                                            uint8_t* buffer_inter_uint = (uint8_t*)(buffer_inter - 12);
                                            Uint2Char(buffer_inter_uint, buffer_inter_uint_char, buff_index1);

                                            /* -------------------------------------------------------------------------- */
                                            /* --- STAGE : 将buff_i的前二十四个字符(必然不会被修改的部分) 与buffer_inter_uint_char的第二十四个字符开始的部分(修改了的部分) 组合起来，转换为uint8_t的buffer_send ---------------------- */


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

                                        /* -------------------------------------------------------------------------- */
										/* --- STAGE : 以两者发送时重复一个rxinfo为代价换取能够单独发送成功---------------------- */
                                    	
                                        memset(buffer1, 0, BUF_SIZE * sizeof(char));
                                        memset(buffer2, 0, BUF_SIZE * sizeof(char));

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

                                    /* -------------------------------------------------------------------------- */
									/* --- STAGE : 以两者发送时重复一个rxinfo为代价换取能够单独发送成功---------------------- */
                                	
                                    memset(buffer1, 0, BUF_SIZE * sizeof(char));
                                    memset(buffer2, 0, BUF_SIZE * sizeof(char));

                                }

                        	}
                        }else if (buff_index1 == 0 && buff_index2 != 0) {
                        	send(sock_up, (void*)buffer2_inter_uint, buff_index2, 0);
                        }else if (buff_index1 != 0 && buff_index2 == 0) {
                        	send(sock_up, (void*)buffer1_inter_uint, buff_index1, 0);
                        }

                    }
                    else if (byte_num < 0)
                    {
                        printf("rescessed error!");
                    }

                    //某个客户端退出
                    else  //cancel fdset and set fd=0
                    {
                        //printf("clien[%d] exit!\n", i);
                        FD_CLR(client_fds[i], &ser_fdset);
                        client_fds[i] = 0;
                        // printf("clien[%d] exit!\n",i);
                        continue;  //这里如果用break的话一个客户端退出会造成服务器也退出。 
                    }
                }
            }
        }


    }
    return 0;

}
