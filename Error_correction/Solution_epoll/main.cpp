#include"Component_1_fakesend/inc/header_1_1.h"
#include"Component_1_getCandidate/inc/header_1_2.h"
#include"Component_1_getMask/inc/header_1_3.h"
#include "Component_1_socketexample/inc/header_1_5.h"

#include "Component_2_server_side/inc/header_2.h"

#include "tools/inc/payload_crc.h"
#include "tools/inc/parson.h"

#include"tools/inc/payload_diff.h"

#include"Component_3_connection_error_correction/inc/header_3.h"

#include "tools/inc/base64.h"

extern int buff_index;

extern char MAC_address1[];
extern char MAC_address2[];
extern int MAC_address_length;

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

    void setData(char* array){
        strcpy(data, array);
    }

    void setIndex(){
        index = strlen(data)/2;
    }

    void setUint(){
        Char2Uint(data, uint);

    }

    virtual void setInter(){
        inter = (char*)(uint + buff_index);
    }

    void setInter_Uint(){
        inter_uint = (uint8_t*)(inter - buff_index);
    }


    void setSize(const char* str){
        size = b64_to_bin(str, strlen(str), payload, sizeof payload);
    }

    virtual void setHexstring(){
        Uint2Char(payload, Hexstring, size);
    }

    virtual void setBinarystring(){
        Hex2Bin(Hexstring, Binarystring);
    }


};

class BufferSend : public Buffer{

public:

    void setInter(char* array){
        strcpy(inter, array);

    }

    void setHexstring(char* array){
        Bin2Hex(array, Hexstring);
    }

    void setBinarystring(char* array1, char* array2){

        if (OZ_bin_xor(array1, array2, Binarystring) != 0) //TODO: Majority voting / more than two copies
        {
            printf("函数出错！\n");
            return;
        }
    }

    void setSend_First_Part_Char(char* array){
        strncpy(send_first_part_char, array, buff_index*2);
        send_first_part_char[strlen(send_first_part_char)] = '\0';
    }

    void setSend_Last_Part_Char(){
        strncpy(send_last_part_char, inter_uint_char + buff_index*2, strlen(inter_uint_char) - buff_index*2);
        send_last_part_char[strlen(send_last_part_char)] = '\0';
    }

    void setSend(){
        strcat(send_first_part_char, send_last_part_char);
        Char2Uint(send_first_part_char, send);
    }

    void setInter_Uint_Char(int num){
        Uint2Char(inter_uint, inter_uint_char, num);
    }

    void setHexstring_uint(){
        Char2Uint(Hexstring, Hexstring_uint8);
    }
};

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

    int breakcount = 0;

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

    int sfd, ss;
    int efd;
    struct epoll_event event;
    struct epoll_event* events;
    char* buff_up_char = new char[BUF_SIZE];

    Buffer buffer1{};
    Buffer buffer2{};
    BufferSend buffer{};

    buffer1.data = new char[BUF_SIZE];
    memset(buffer1.data, 0, BUF_SIZE * sizeof(char));
    buffer2.data = new char[BUF_SIZE];
    memset(buffer2.data, 0, BUF_SIZE * sizeof(char));

    sfd = create_and_bind();
    if (sfd == -1)
        abort();

    ss = make_socket_non_blocking(sfd);
    if (ss == -1)
        abort();

    ss = listen(sfd, SOMAXCONN);
    if (ss == -1)
    {
        perror("listen");
        abort();
    }

    efd = epoll_create1(0);
    if (efd == -1)
    {
        perror("epoll_create");
        abort();
    }

    event.data.fd = sfd;
    event.events = EPOLLIN | EPOLLET;
    ss = epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &event);
    if (ss == -1)
    {
        perror("epoll_ctl");
        abort();
    }

    /* Buffer where events are returned */
    //events = calloc(MAXEVENTS, sizeof event);
    events = new epoll_event[MAXEVENTS];
    memset(events, 0, MAXEVENTS * sizeof(epoll_event));

    /* The event loop */
    while (1)
    {
        int n, i;

        n = epoll_wait(efd, events, MAXEVENTS, -1);
        for (i = 0; i < n; i++)
        {
            if ((events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP) ||
                (!(events[i].events & EPOLLIN)))
            {
                /* An error has occured on this fd, or the socket is not
                         ready for reading (why were we notified then?) */
                fprintf(stderr, "epoll error\n");
                close(events[i].data.fd);
                continue;
            }

            else if (sfd == events[i].data.fd)
            {
                /* We have a notification on the listening socket, which
                         means one or more incoming connections. */
                while (1)
                {
                    struct sockaddr in_addr;
                    socklen_t in_len;
                    int infd;
                    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

                    in_len = sizeof in_addr;
                    infd = accept(sfd, &in_addr, &in_len);
                    if (infd == -1)
                    {
                        if ((errno == EAGAIN) ||
                            (errno == EWOULDBLOCK))
                        {
                            /* We have processed all incoming
                                           connections. */
                            break;
                        }
                        else
                        {
                            perror("accept");
                            break;
                        }
                    }

                    ss = getnameinfo(&in_addr, in_len,
                                     hbuf, sizeof hbuf,
                                     sbuf, sizeof sbuf,
                                     NI_NUMERICHOST | NI_NUMERICSERV);
                    if (ss == 0)
                    {
                        //printf("Accepted connection on descriptor %d "
                        //       "(host=%s, port=%s)\n",
                        //       infd, hbuf, sbuf);
                    }

                    /* Make the incoming socket non-blocking and add it to the
                               list of fds to monitor. */
                    ss = make_socket_non_blocking(infd);
                    if (ss == -1)
                        abort();

                    event.data.fd = infd;
                    event.events = EPOLLIN | EPOLLET;
                    ss = epoll_ctl(efd, EPOLL_CTL_ADD, infd, &event);
                    if (ss == -1)
                    {
                        perror("epoll_ctl");
                        abort();
                    }
                }
                continue;
            }
            else
            {
                /* We have data on the fd waiting to be read. Read and
                         display it. We must read whatever data is available
                         completely, as we are running in edge-triggered mode
                         and won't get a notification again for the same
                         data. */
                int done = 0;

                while (1)
                {
                    breakcount++;
                    ssize_t count;
                    memset(buff_up_char, 0, BUF_SIZE * sizeof(char));
                    count = read(events[i].data.fd, buff_up_char, BUF_SIZE * sizeof buff_up_char);

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

#if DEBUG
                    printf("breakcount: %d\n\n", breakcount);
#endif

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 使用breakcount控制不发送重复数据---------------------- */

                    if (breakcount % 2 == 1) {

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
                        /* --- STAGE : epoll的异步处理---------------------- */
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
#endif
                                        int i = 0;
#if DEBUG
                                        printf("Mask: %s\n", s);
#endif

                                        int Hamming_weight_max = 30; //预设的最多纠错比特位数量
                                        if (Hamming_weight_now > Hamming_weight_max) {

                                            printf("%s: %d\n", "Hamming weight is larger than the max number", Hamming_weight_max);
                                            printf("This program will be shut down!\n"); //TODO: 一个单包一个多包接收必然会出现这种情况
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

                                        if(Hamming_weight_now <= Hamming_weight_max/2){
                                            incremental_correct(buffer.Binarystring, mch, Hamming_weight_now, crc_int, fakeresult, realresult, size, pass_crc, total_number);
                                        }else{
                                            correct(buffer.Binarystring, mch, Hamming_weight_now, crc_int, fakeresult, realresult, size, pass_crc, total_number);
                                        }

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

                                            buffer.setInter(buffer1.inter); //将bufferi_inter赋值buffer_inter给以后续处理
                                            buffer.setInter_Uint();
                                            buffer.setInter_Uint_Char(buffer1.index);

                                            /* -------------------------------------------------------------------------- */
                                            /* --- STAGE : 将buff_i的前12-byte(必然不会被修改的header部分) 与buffer_inter_uint_char的第12 byte开始的部分(修改后的Upstream JSON data structure) 组合起来，转换为uint8_t的buffer_send ---------------------- */


                                            buffer.setSend_First_Part_Char(buffer1.data);
                                            buffer.setSend_Last_Part_Char();
                                            buffer.setSend();


#if DEBUG
                                            printf("buffer.send: ");
                                            for (int count = 0; count < buffer1.index; count++) {
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
                                            for (int count = 0; count < buffer2.index; count++) {
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
                            send(sock_up, (void*)buffer2.inter_uint, buffer2.index, 0);

                        }
                        else if (buffer1.index != 0 && buffer2.index == 0) {
                            send(sock_up, (void*)buffer1.inter_uint, buffer1.index, 0);

                        }
                    }

                    if (count == -1)
                    {
                        /* If errno == EAGAIN, that means we have read all
                                     data. So go back to the main loop. */
                        if (errno != EAGAIN)
                        {
                            perror("read");
                            done = 1;
                        }
                        break;
                    }
                    else if (count == 0)
                    {
                        /* End of file. The remote has closed the
                                     connection. */
                        done = 1;
                        break;
                    }
                    //char hello[] = "Hello! Are You Fine?\n";
                    //write(events[i].data.fd, hello, strlen(hello));
                    /* Write the buffer to standard output */
                    /*测试代码
                    ss = write(1, buff_up_char, count);
                    if (ss == -1)
                    {
                        perror("write");
                        abort();
                    }
                    */
                }

                if (done)
                {
                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 发送---------------------- */

                    //printf("Closed connection on descriptor %d\n",
                    //       events[i].data.fd);

                    /* Closing the descriptor will make epoll remove it
                               from the set of descriptors which are monitored. */
                    close(events[i].data.fd);
                }
            }
        }
    }

    delete[] buff_up_char;

    free(events);

    close(sfd);

    return EXIT_SUCCESS;


}

