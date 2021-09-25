//https://www.cnblogs.com/wuyepeng/p/9726771.html

#include"header_1_1.h"
#include"header_1_2.h"
#include"header_1_3.h"
#include "payload_crc.h"
#include "header_2.h"
#include"header_1_6.h"
#include "parson.h"

#include"payload_diff.h"

#include"header_3.h"

#include "base64.h"

extern int sock_up;

int buff_index = 12;

extern char MAC_address1[];
extern char MAC_address2[];
extern int MAC_address_length;


int main() {


    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 建立发射socket ---------------------- */

    int i = create_up_socket();
    if (i==-1) abort();

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 开始处理数据 ---------------------- */

    int ser_souck_fd;

    char input_message[BUF_SIZE] = {0};
    char* buff_up_char = new char[BUF_SIZE];

    Buffer buffer1{};
    Buffer buffer2{};
    BufferSend buffer{};

    buffer1.data = new char[BUF_SIZE];
    memset(buffer1.data, 0, BUF_SIZE * sizeof(char));
    buffer2.data = new char[BUF_SIZE];
    memset(buffer2.data, 0, BUF_SIZE * sizeof(char));

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
                    memset(buff_up_char, 0, BUF_SIZE * sizeof(char));
                    int byte_num = read(client_fds[i], buff_up_char, BUF_SIZE);
                    if (byte_num > 0)
                    {
                        //printf("message form client[%d]:%s\n", i, buff_up_char);

                        char* Gateway_unique_identifier = new char[MAC_address_length];
                        memset(Gateway_unique_identifier, 0, MAC_address_length * sizeof(char));
                        Gateway_unique_identifier[MAC_address_length]='\0';
                        strncpy(Gateway_unique_identifier, buff_up_char+MAC_address_length/2, MAC_address_length);
                        if(strcmp(Gateway_unique_identifier,MAC_address1)==0){
                            buffer1.setData(buff_up_char);
                        }else if(strcmp(Gateway_unique_identifier,MAC_address2)==0){
                            buffer2.setData(buff_up_char);
                        }

                        //printf("buffer1: %s\n", buffer1.data);
                        //TODO: 检查socketexample初始时单个网关是否会导致多接收/以及这里的buffer是否，若是则需要换框架
                        //printf("buffer2: %s\n", buffer2.data);

                        buffer1.setIndex();
                        buffer2.setIndex();

                        buffer1.uint[BUF_SIZE] = {0};
                        buffer2.uint[BUF_SIZE] = {0};

                        buffer1.setUint();
                        buffer2.setUint();

                        /* -------------------------------------------------------------------------- */
                        /* --- STAGE : 对中间数据buffer_inter纠错---------------------- */
                        //TODO: false and true带来的多个包同时转发


                        buffer1.setInter(); //接收到的Upstream JSON data structure
                        buffer2.setInter();
                        //printf("buffer1.inter: %s\n", buffer1.inter);
                        //printf("\n");
                        //printf("buffer2.inter: %s\n", buffer2.inter);
                        //printf("\n");

                        buffer1.setInter_Uint();
                        buffer2.setInter_Uint();

                        /* -------------------------------------------------------------------------- */
                        /* --- STAGE : select的异步处理---------------------- */
                        Rxpk rxpk1{};
                        Rxpk rxpk2{};

                        rxpk1.setTime(buffer1.uint,buff_index);
                        rxpk2.setTime(buffer2.uint,buff_index);

                        if (buffer1.index != 0 && buffer2.index != 0) {

                            if (strcmp(rxpk1.time, rxpk2.time) == 0)
                            {
                                /* -------------------------------------------------------------------------- */
                                /* --- STAGE : 找到上行数据中需要的属性的值 ---------------------- */
                                //TODO: 解决多数据包同时上行情况 (重复数据包接收是因为距离太近)
                                //https://forum.rakwireless.com/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/3980/3?u=haowong
                                //https://forum.chirpstack.io/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/10886/2?u=shirou_emiya

                                rxpk1.setStat(buffer1.uint,buff_index);
                                rxpk2.setStat(buffer2.uint,buff_index);

                                rxpk1.setCrc_get(buffer1.uint,buff_index);
                                rxpk2.setCrc_get(buffer2.uint,buff_index);
                                unsigned int crc_get = 0;

                                rxpk1.setStr(buffer1.uint,buff_index);
                                rxpk2.setStr(buffer2.uint,buff_index);

                                rxpk1.setRssi(buffer1.uint,buff_index);
                                rxpk2.setRssi(buffer2.uint,buff_index);

#if DEBUG
                                printf("rxpk1.stat: %d\n", rxpk1.stat);
                                printf("rxpk1.crc_get: %d\n", rxpk1.crc_get);
                                printf("rxpk1.str: %s\n", rxpk1.str);
                                printf("rxpk1.rssi: %d\n", rxpk1.rssi);
                                printf("rxpk1.time: %s\n", rxpk1.time);
#endif


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

                                        buffer1.setSize(rxpk1.str); //与net_downlink相似，都是接收到data，故都用b64_to_bin
#if DEBUG
                                        printf("Copy_1 of data: %s\n", rxpk1.str);
#endif
                                        delete[] rxpk1.str;


                                        buffer2.payload[BUF_SIZE] = {0};

                                        buffer2.setSize(rxpk2.str); //与net_downlink相似，都是接收到data，故都用b64_to_bin
#if DEBUG
                                        printf("Copy_2 of data: %s\n", rxpk2.str);
#endif
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

#if DEBUG
                                        printf("Hamming_weight_now: %d\n", Hamming_weight_now);

#endif

                                        /* -------------------------------------------------------------------------- */
                                        /* --- STAGE : uint8_t转char ---------------------- */ //https://bbs.csdn.net/topics/390141308

                                        buffer1.Hexstring = new char[BUF_SIZE];
                                        memset(buffer1.Hexstring, 0, BUF_SIZE * sizeof(char));

                                        buffer1.setHexstring();
#if DEBUG
                                        printf("M's: %s\n", buffer1.Hexstring);
#endif

                                        buffer2.Hexstring = new char[BUF_SIZE];
                                        memset(buffer2.Hexstring, 0, BUF_SIZE * sizeof(char));

                                        buffer2.setHexstring();
#if DEBUG
                                        printf("M'r: %s\n", buffer2.Hexstring);
#endif


                                        /* -------------------------------------------------------------------------- */
                                        /* --- STAGE : 十六进制字符串转二进制字符串 ---------------------- */ //https://blog.csdn.net/weixin_30279751/article/details/95437814

                                        buffer1.Binarystring = new char[BUF_SIZE];
                                        memset(buffer1.Binarystring, 0, BUF_SIZE * sizeof(char));
                                        buffer2.Binarystring = new char[BUF_SIZE];
                                        memset(buffer2.Binarystring, 0, BUF_SIZE * sizeof(char));

                                        buffer1.setBinarystring();
                                        buffer2.setBinarystring();
                                        delete[] buffer1.Hexstring;
                                        delete[] buffer2.Hexstring;

                                        /* -------------------------------------------------------------------------- */
                                        /* --- STAGE : 二进制字符串异或 ---------------------- */

                                        buffer.Binarystring = new char[BUF_SIZE]; //Merged error mask / Ambiguity vectors / Va
                                        memset(buffer.Binarystring, 0, BUF_SIZE * sizeof(char));

                                        buffer.setBinarystring(buffer1.Binarystring, buffer2.Binarystring);

                                        /* -------------------------------------------------------------------------- */
                                        /* --- STAGE : GetCandidate ---------------------- */
                                        /* -------------------------------------------------------------------------- */
                                        /* --- STAGE : CRC ---------------------- */


                                        char* mch = new char[BUF_SIZE];
                                        memset(mch, 0, BUF_SIZE * sizeof(char));
                                        if (rxpk1.rssi >= rxpk2.rssi) { //Selection Combining (SC)

                                            strcpy(mch, buffer1.Binarystring);

                                        }
                                        else {

                                            strcpy(mch, buffer2.Binarystring);

                                        }

                                        delete[] buffer1.Binarystring;
                                        delete[] buffer2.Binarystring;
#if DEBUG
                                        printf("MCH: %s\n", mch);
#endif

                                        char* crc = new char[BUF_SIZE];
                                        memset(crc, 0, BUF_SIZE * sizeof(char));
                                        sprintf(crc, "0x%04X", crc_get);
#if DEBUG
                                        printf("Processed CRC: %s\n", crc);
#endif
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
                                            printf("This program will be shut down!\n"); //TODO: 一个单包一个多包接收必然会出现这种情况
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
#if DEBUG
                                        printf("Corrected data: %s\n", data_up);
#endif
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

                                        if (rxpk1.rssi >= rxpk2.rssi) {


                                            /* -------------------------------------------------------------------------- */
                                            /* --- STAGE : 将Upstream JSON data structure的"data" field里面的数据使用修改后的data_up覆盖 ---------------------- */

                                            strncpy(buffer1.inter + FindFirstSubchar(buffer1.inter, "data") + 6, data_up, strlen(data_up)); //https://blog.csdn.net/zmhawk/article/details/44600075

#if DEBUG
                                            //TODO: JSON serialization

                                            JSON_Value* root_val = NULL;
                                            JSON_Object* first_obj = NULL;
                                            JSON_Array* rxpk_array = NULL;

                                            root_val = json_parse_string_with_comments((const char*)(buffer1.uint + buff_index));
                                            rxpk_array = json_object_get_array(json_value_get_object(root_val), "rxpk");
                                            first_obj = json_array_get_object(rxpk_array, 0);
                                            json_object_set_string(first_obj, "data", data_up);
                                            buffer1.inter = json_serialize_to_string(root_val);
                                            puts(buffer1.inter);
#endif

                                            /* -------------------------------------------------------------------------- */
                                            /* --- STAGE : 更改stat从-1到1 ---------------------- */

                                            deleteChar(buffer1.inter, FindFirstSubchar(buffer1.inter, "stat") + 5);
                                            buffer1.index--;

                                            /* -------------------------------------------------------------------------- */
                                            /* --- STAGE : 构造出前12-byte header缺陷的buffer_inter_uint_char ---------------------- */

                                            buffer.setInter(buffer1.inter);
                                            buffer.setInter_Uint();
                                            buffer.setInter_Uint_Char(buffer1.index);

                                            /* -------------------------------------------------------------------------- */
                                            /* --- STAGE : 将buff_i的前12-byte(必然不会被修改的header部分) 与buffer_inter_uint_char的第12 byte开始的部分(修改后的Upstream JSON data structure) 组合起来，转换为uint8_t的buffer_send ---------------------- */


                                            buffer.setSend_First_Part_Char(buffer1.data);
                                            buffer.setSend_Last_Part_Char();
                                            buffer.setSend();

#if DEBUG
                                            printf("buffer.send: ");
                                        for (int loopcount = 0; loopcount < buffer_array[0].index; loopcount++) {
                                            printf("%02X", buffer.send[count]);
                                        }
                                        printf("\n\n");
#endif


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
                                            buffer.setInter(buffer2.inter);
                                            buffer.setInter_Uint();
                                            buffer.setInter_Uint_Char(buffer2.index);
                                            buffer.setSend_First_Part_Char(buffer2.data);
                                            buffer.setSend_Last_Part_Char();
                                            buffer.setSend();

#if DEBUG
                                            printf("buffer.send: ");
                                        for (int loopcount = 0; loopcount < buffer_array[0].index; loopcount++) {
                                            printf("%02X", buffer.send[count]);
                                        }
                                        printf("\n\n");
#endif


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

#if DEBUG
                                        printf("buffer_send1: ");
                                        for (int loopcount = 0; loopcount < buffer_array[0].index; loopcount++) {
                                        printf("%02X", buffer_array[0].inter_uint[loopcount]);
                                        }
                                        printf("\n\n");

                                        printf("buffer_send2: ");
                                        for (int loopcount = 0; loopcount < buffer_array[1].index; loopcount++) {
                                        printf("%02X", buffer_array[1].inter_uint[loopcount]);
                                        }
                                        printf("\n\n");
#endif


                                        /* -------------------------------------------------------------------------- */
                                        /* --- STAGE : 发送---------------------- */


                                        send(sock_up, (void*)buffer1.inter_uint, buffer1.index, 0);
                                        send(sock_up, (void*)buffer2.inter_uint, buffer2.index, 0);

                                        /* -------------------------------------------------------------------------- */
                                        /* --- STAGE : 以两者发送时重复一个rxinfo为代价换取能够单独发送成功---------------------- */

                                        memset(buffer1.data, 0, BUF_SIZE * sizeof(char));
                                        memset(buffer2.data, 0, BUF_SIZE * sizeof(char));

                                    }

                                }
                                else {

                                    printf("At least one packet is crc correct, no operation will be taken\n\n");

#if DEBUG
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
#endif


                                    /* -------------------------------------------------------------------------- */
                                    /* --- STAGE : 发送---------------------- */


                                    send(sock_up, (void*)buffer1.inter_uint, buffer1.index, 0);
                                    send(sock_up, (void*)buffer2.inter_uint, buffer2.index, 0);

                                    /* -------------------------------------------------------------------------- */
                                    /* --- STAGE : 以两者发送时重复一个rxinfo为代价换取能够单独发送成功---------------------- */

                                    memset(buffer1.data, 0, BUF_SIZE * sizeof(char));
                                    memset(buffer2.data, 0, BUF_SIZE * sizeof(char));

                                }

                            }
                        }
                        else if (buffer1.index == 0 && buffer2.index != 0) {
                            printf("Only the %s gateway received message and transferd it\n\n", MAC_address2);
                            send(sock_up, (void*)buffer2.inter_uint, buffer2.index, 0);

                        }
                        else if (buffer1.index != 0 && buffer2.index == 0) {
                            printf("Only the %s gateway received message and transferd it\n\n", MAC_address1);
                            send(sock_up, (void*)buffer1.inter_uint, buffer1.index, 0);

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