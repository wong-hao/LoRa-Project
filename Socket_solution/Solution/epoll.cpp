#include"header_1_1.h"
#include"header_1_2.h"
#include"header_1_3.h"
#include "payload_crc.h"
#include"header_1_5.h"
#include "parson.h"

#include"payload_diff.h"
#include"header_2_2.h"

#include"header_3.h"

#include "base64.h"

void outmystr(int n, char* input, int compare, char* interoutput, char* finaloutput, int length, int& flag, int& test) //https://bbs.csdn.net/topics/399153127
{
    if (flag == 1) {

        return; //flag=1说明已经有一个crc校验通过的了，直接退出，这样会直接根除掉假阳性false positives (Hash碰撞)

    }

    OZ_bin_xor(input, d, interoutput);

    char* Hexstring_temp = new char[BUF_SIZE];
    memset(Hexstring_temp, 0, BUF_SIZE * sizeof(char));

    uint8_t* Hexstring_uint8_temp = new uint8_t[BUF_SIZE];
    memset(Hexstring_uint8_temp, 0, BUF_SIZE * sizeof(uint8_t)); //目前看来是最关键的动态数组，如果静态就会堆栈报错

    uint16_t    payload_crc16_calc_temp = 0;

    Bin2Hex(interoutput, Hexstring_temp);

    Char2Uint(Hexstring_temp, Hexstring_uint8_temp);
    delete[] Hexstring_temp;

    payload_crc16_calc_temp = sx1302_lora_payload_crc(Hexstring_uint8_temp, length);
    delete[] Hexstring_uint8_temp;

    if (n < 0) {

        if (payload_crc16_calc_temp == compare) {
            strcpy(finaloutput, interoutput);

            flag++;

            /* 测试代码

            printf("Pass crc check time: %d and it happends at candidate number: %d\n", flag, test);
            printf("The Passed InterPHYPayload : ");
            for (int count = 0; count < length; count++) {
                printf("%02X", Hexstring_uint8_temp[count]);
            }
            printf("\n");

            char* Hexstring4 = new char[BUF_SIZE]; //char类型的PHYPayload
            memset(Hexstring4, 0, BUF_SIZE * sizeof(char));
            Bin2Hex(interoutput, Hexstring4);
            uint8_t* Hexstring4_uint8 = new uint8_t[BUF_SIZE];
            memset(Hexstring4_uint8, 0, BUF_SIZE * sizeof(uint8_t));
            Char2Uint(Hexstring4, Hexstring4_uint8);
            uint8_t* data_up_uint8 = new uint8_t[BUF_SIZE]; //不用太大， 因为原代码里的buff_up不止装的data所以很大
            memset(data_up_uint8, 0, BUF_SIZE * sizeof(uint8_t));
            bin_to_b64(Hexstring4_uint8, length, (char*)(data_up_uint8), BUF_SIZE);
            char* data_up = new char[BUF_SIZE]; //char类型的PHYPayload
            memset(data_up, 0, BUF_SIZE * sizeof(char));
            strcpy(data_up, (char*)(data_up_uint8));
            printf("The Passed Interresult: %s\n", data_up);

            printf("\n");
            printf("The passed Payload CRC (0x%04X)\n", payload_crc16_calc_temp);
            */

            //TODO: hidden errors
            //TODO: 超时退出程序：//https://blog.csdn.net/codedz/article/details/80387001, 其实可以不用，最大纠错比特位数量Hamming_weight_max已经可以用来限制了
        }

        /* 测试代码
        printf("Candidate: %s\n", d);
        printf("Interoutput: %s\n", interoutput);
        */

        /* 测试代码
        test++;

        printf("The number of candidate: %d\n", test);
        printf("Every InterPHYPayload: \n");
        for (int count = 0; count < length; count++) {
            printf("%02X", Hexstring_uint8_temp[count]);
        }
        printf("Every Payload CRC (0x%04X)\n", payload_crc16_calc_temp);
        printf("\n");
        */
    }
    else
    {
        d[n] = '0';
        outmystr(n - 1, input, compare, interoutput, finaloutput, length, flag, test);
        if (s[n] == '1')
        {
            d[n] = '1';
            outmystr(n - 1, input, compare, interoutput, finaloutput, length, flag, test);
        }

    }

}


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
    char* buf = new char[BUF_SIZE];

    char* buffer1 = new char[BUF_SIZE];
    memset(buffer1, 0, BUF_SIZE * sizeof(char));
    char* buffer2 = new char[BUF_SIZE];
    memset(buffer2, 0, BUF_SIZE * sizeof(char));

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
                    memset(buf, 0, BUF_SIZE * sizeof(char));
                    count = read(events[i].data.fd, buf, BUF_SIZE * sizeof buf);

                    if (buf[23] == '6') {
                        strcpy(buffer1, buf);
                    }
                    else if (buf[23] == '7') {
                        strcpy(buffer2, buf);
                    }

                    //printf("buffer1: %s\n", buffer1);

                    //printf("buffer2: %s\n", buffer2);

                    int buff_index1 = strlen(buffer1) / 2;
                    int buff_index2 = strlen(buffer2) / 2;

                    uint8_t  buffer_uint1[BUF_SIZE] = { 0 };
                    uint8_t  buffer_uint2[BUF_SIZE] = { 0 };
                    Char2Uint(buffer1, buffer_uint1);
                    Char2Uint(buffer2, buffer_uint2);

                	/*测试代码
                    printf("breakcount: %d\n\n", breakcount);
                    */

                    /* -------------------------------------------------------------------------- */
					/* --- STAGE : 使用breakcount控制不发送重复数据---------------------- */
                	
                    if (breakcount % 2 == 1) {

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

                        char report10[BUF_SIZE] = "time";
                        char report11[BUF_SIZE] = "tmms";

                        /* -------------------------------------------------------------------------- */
                        /* --- STAGE : epoll的异步处理---------------------- */
                    	
                        char* time1 = new char[BUF_SIZE];
                        memset(time1, 0, BUF_SIZE * sizeof(char));
                        char* time2 = new char[BUF_SIZE];
                        memset(time2, 0, BUF_SIZE * sizeof(char));

                        if (buff_index1 != 0 && buff_index2 != 0) {
                            getTime(time1, buffer1_inter, report10, report11); //TODO: 将涉及到json的地方从header_2_2改成parson（包括lora_pkt_fwd.c）
                            getTime(time2, buffer2_inter, report10, report11);

                            if (strcmp(time1, time2) == 0)
                            {
                                /* -------------------------------------------------------------------------- */
                                 /* --- STAGE : 找到上行数据中需要的属性的值 ---------------------- */
                                 //TODO: 解决多数据包同时上行情况 (重复数据包接收是因为距离太近)
                                 //https://forum.rakwireless.com/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/3980/3?u=haowong
                                 //https://forum.chirpstack.io/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/10886/2?u=shirou_emiya
                                char report1[BUF_SIZE] = "stat";
                                char report2[BUF_SIZE] = "crc";
                                char report3[BUF_SIZE] = "modu";
                                char report4[BUF_SIZE] = "data";
                                char report5[BUF_SIZE] = "rssis";
                                char report6[BUF_SIZE] = "lsnr";
                                char report7[BUF_SIZE] = "}";
                                char report8[BUF_SIZE] = "rssi";
                                char report9[BUF_SIZE] = "size";


                                char* stat1 = new char[BUF_SIZE];
                                memset(stat1, 0, BUF_SIZE * sizeof(char));
                                getStat(stat1, buffer1_inter, report1, report2);
                                char* stat2 = new char[BUF_SIZE];
                                memset(stat2, 0, BUF_SIZE * sizeof(char));
                                getStat(stat2, buffer2_inter, report1, report2);

                                char* crc_get1 = new char[BUF_SIZE];
                                memset(crc_get1, 0, BUF_SIZE * sizeof(char));
                                getCrc(crc_get1, buffer1_inter, report2, report3);
                                char* crc_get2 = new char[BUF_SIZE];
                                memset(crc_get2, 0, BUF_SIZE * sizeof(char));
                                getCrc(crc_get2, buffer2_inter, report2, report3);
                                char* crc_get = new char[BUF_SIZE];
                                memset(crc_get, 0, BUF_SIZE * sizeof(char));

                                char* str1 = new char[BUF_SIZE];
                                memset(str1, 0, BUF_SIZE * sizeof(char));
                                getStr(str1, buffer1_inter, report4, report7);
                                char* str2 = new char[BUF_SIZE];
                                memset(str2, 0, BUF_SIZE * sizeof(char));
                                getStr(str2, buffer2_inter, report4, report7);

                                /*测试代码
                                char* rssis1 = new char[BUF_SIZE];
                                memset(rssis1, 0, BUF_SIZE * sizeof(char));
                                getRssis(rssis1, buffer1_inter, report5, report6);
                                char* rssis2 = new char[BUF_SIZE];
                                memset(rssis2, 0, BUF_SIZE * sizeof(char));
                                getRssis(rssis2, buffer2_inter, report5, report6);
                                */

                                char* rssi1 = new char[BUF_SIZE];
                                memset(rssi1, 0, BUF_SIZE * sizeof(char));
                                getRssi(rssi1, buffer1_inter, report8, report9);
                                char* rssi2 = new char[BUF_SIZE];
                                memset(rssi2, 0, BUF_SIZE * sizeof(char));
                                getRssi(rssi2, buffer2_inter, report8, report9);

                                /*测试代码
                                printf("stat1: %s\n", stat1);
                                printf("crc_get1: %s\n", crc_get1);
                                printf("str1: %s\n", str1);
                                printf("rssis1: %s\n", rssis1);
                                printf("time1: %s\n", time1);

                                */


                                /* -------------------------------------------------------------------------- */
                                /* --- STAGE : 当两个上行数据都错且crc值相同时进行纠错 ---------------------- */


                                if ((atoi(stat1) == -1) && (atoi(stat2) == -1)) {

                                    delete[] stat1;
                                    delete[] stat2;

                                    printf("Both two packets are crc incorrect\n");

                                    if ((atoi(crc_get1) == atoi(crc_get2))) {

                                        printf("Both two packets have the same FCS\n\n");

                                        printf("Error correction begins\n\n");

                                        strcpy(crc_get, crc_get1);
                                        delete[] crc_get1;
                                        delete[] crc_get2;
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
                                        if (atof(rssi1) >= atof(rssi2)) {

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
                                        unsigned int crc_buffer = atoi(crc_get);
                                        delete[] crc_get;
                                        char* crc = new char[BUF_SIZE];
                                        memset(crc, 0, BUF_SIZE * sizeof(char));
                                        sprintf(crc, "0x%04X", crc_buffer);
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

                                        if (atof(rssi1) >= atof(rssi2)) {


                                            /* -------------------------------------------------------------------------- */
                                            /* --- STAGE : 构造出前24个字符缺陷的buffer_inter_uint_char：替换data_up ---------------------- */

                                            strncpy(buffer1_inter + FindFirstSubchar(buffer1_inter, report4) + 6, data_up, strlen(data_up)); //https://blog.csdn.net/zmhawk/article/details/44600075

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

                                            //delete[] rssis1;
                                            //delete[] rssis2;
                                            delete[] rssi1;
                                            delete[] rssi2;
                                            delete[] data_up;
                                            delete[] buffer_inter;
                                            delete[] buffer_inter_uint_char;


                                            /* -------------------------------------------------------------------------- */
                                            /* --- STAGE : 发送---------------------- */


                                            send(sock_up, (void*)buffer_send, buff_index1, 0);

                                        }
                                        else {

                                            strncpy(buffer2_inter + FindFirstSubchar(buffer2_inter, report4) + 6, data_up, strlen(data_up));
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

                                           //delete[] rssis1;
                                           //delete[] rssis2;
                                            delete[] rssi1;
                                            delete[] rssi2;
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
                        }
                        else if (buff_index1 == 0 && buff_index2 != 0) {
                            send(sock_up, (void*)buffer2_inter_uint, buff_index2, 0);

                        }
                        else if (buff_index1 != 0 && buff_index2 == 0) {
                            send(sock_up, (void*)buffer1_inter_uint, buff_index1, 0);

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
                    ss = write(1, buf, count);
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

    delete[] buf;

    free(events);

    close(sfd);

    return EXIT_SUCCESS;


}

