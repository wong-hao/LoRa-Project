#include"header_1_1.h"
#include"header_1_2.h"
#include"header_1_3.h"
#include "header_1_5.h"

#include "header_2.h"

#include "parson.h"
#include "payload_crc.h"
#include"payload_diff.h"

#include"header_3.h"

#include "base64.h"

extern int sock_up;

extern char MAC_address1[];
extern char MAC_address2[];
extern char MAC_address3[];
extern char MAC_address4[];
extern int MAC_address_length;

int main() {

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : epoll特有的控制socket个数方法初始化，以免同一个case重复执行多次（副作用是可能多次都无法收集到time相同的数据包导致无法转发） ---------------------- */

    int breakcount = 0;

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 建立发射socket ---------------------- */

    int i = create_up_socket();
    if (i==-1) abort();

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 开始处理数据 ---------------------- */

    int buffer_num = 2;
    Buffer buffer_array[buffer_num];

    BufferSend buffer{};

    for(int loopcount=0; loopcount<=buffer_num-1; loopcount++){
        buffer_array[loopcount].data = new char[BUF_SIZE];
        memset(buffer_array[loopcount].data, 0, BUF_SIZE * sizeof(char));
    }

    double CRCErrorNum = 0;
    double NonCRCErrorNum = 0;
    double PER; //计算未通过CRC校验的全局PER
    double PDR;

    int sfd, ss;
    int efd;
    struct epoll_event event;
    struct epoll_event* events;
    char* buff_up_char = new char[BUF_SIZE];

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
#if DEBUG
                        char replacement[] = "4A";
                            string s1(buff_up_char);
                            s1.replace(s1.length() - 18, strlen(replacement), replacement);

                            memset(buff_up_char, 0, BUF_SIZE*sizeof(char));

                            strcpy(buff_up_char,s1.c_str());
                            buff_up_char[strlen(buff_up_char)] = '\0';
#endif
                        buffer_array[0].setData(buff_up_char);
                    }else if(strcmp(Gateway_unique_identifier,MAC_address2)==0){
#if DEBUG
                        char replacement[] = "6B";
                            string s1(buff_up_char);
                            s1.replace(s1.length() - 22, strlen(replacement), replacement);

                            memset(buff_up_char, 0, BUF_SIZE*sizeof(char));

                            strcpy(buff_up_char,s1.c_str());
                            buff_up_char[strlen(buff_up_char)] = '\0';
#endif
                        buffer_array[1].setData(buff_up_char);
                    }

#if DEBUG
                    //TODO: 检查socketexample初始时单个网关是否会导致多接收/以及这里的buffer是否，若是则需要换框架
                        for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                            cout<<"buffer"<<loopcount+1<<": "<<buffer_array[loopcount].data<<endl;
                        }

                        printf("\n");
#endif

                    for(int loopcount=0; loopcount<=buffer_num-1; loopcount++){
                        buffer_array[loopcount].setIndex();
                        buffer_array[loopcount].uint[BUF_SIZE] = {0};
                        buffer_array[loopcount].setUint();
                    }

#if DEBUG
                    printf("breakcount: %d\n\n", breakcount);
#endif

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 使用breakcount控制不发送重复数据---------------------- */

                    if (breakcount % 2 == 1) {

                        /* -------------------------------------------------------------------------- */
                        /* --- STAGE : 对中间数据buffer_inter纠错---------------------- */
                        //TODO: false and true带来的多个包同时转发


                        for(int loopcount=0; loopcount<=buffer_num-1; loopcount++){
                            buffer_array[loopcount].inter = new char[BUF_SIZE];
                            memset(buffer_array[loopcount].inter, 0, BUF_SIZE * sizeof(char));
                            buffer_array[loopcount].setInter(); //接收到的Upstream JSON data structure
#if DEBUG
                            cout<<"buffer"<<loopcount+1<<".inter: "<<buffer_array[loopcount].inter<<endl;
#endif
                            buffer_array[loopcount].inter_uint = new uint8_t [BUF_SIZE];
                            memset(buffer_array[loopcount].inter_uint, 0, BUF_SIZE * sizeof(uint8_t));
                            buffer_array[loopcount].setInter_Uint();
                        }

                        /* -------------------------------------------------------------------------- */
                        /* --- STAGE : epoll的异步处理---------------------- */

                        Rxpk rxpk_array[buffer_num];

                        for(int loopcount=0; loopcount<=buffer_num-1; loopcount++)  rxpk_array[loopcount].setTime(buffer_array[loopcount].uint,buffer_array->buff_index);

                        switch(countED(buffer_array, buffer_num)){
                            case 2:{
                                if (compareTime(rxpk_array, buffer_num)){

#if DEBUG
                                printf("buffer_send1: ");
                                for (int count = 0; count < buffer_array[0].index; count++) {
                                    printf("%02X", buffer_array[0].inter_uint[count]);
                                }
                                printf("\n");

                                printf("buffer_send2: ");
                                for (int count = 0; count < buffer_array[1].index; count++) {
                                    printf("%02X", buffer_array[1].inter_uint[count]);
                                }
                                printf("\n");


                                for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                                    cout<<"buffer"<<loopcount+1<<".inter: "<<buffer_array[loopcount].inter<<endl;
                                    send(sock_up, (void*)buffer_array[loopcount].inter_uint, buffer_array[loopcount].index, 0);
                                }

                                printf("\n");


#endif
                                    /* -------------------------------------------------------------------------- */
                                    /* --- STAGE : 找到上行数据中需要的属性的值 ---------------------- */
                                    //TODO: 解决多数据包同时上行情况 (重复数据包接收是因为距离太近)
                                    //https://forum.rakwireless.com/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/3980/3?u=haowong
                                    //https://forum.chirpstack.io/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/10886/2?u=shirou_emiya

                                    for(int loopcount=0; loopcount<=buffer_num-1; loopcount++){
                                        rxpk_array[loopcount].setStat(buffer_array[loopcount].uint,buffer_array->buff_index);
                                        rxpk_array[loopcount].setCrc_get(buffer_array[loopcount].uint,buffer_array->buff_index);
                                        rxpk_array[loopcount].setStr(buffer_array[loopcount].uint,buffer_array->buff_index);
                                        rxpk_array[loopcount].setRssi(buffer_array[loopcount].uint,buffer_array->buff_index);
                                    }

#if DEBUG
                                    printf("rxpk1.stat: %d\n", rxpk_array[0].stat);
                                printf("rxpk1.crc_get: %d\n", rxpk_array[0].crc_get);
                                printf("rxpk1.str: %s\n", rxpk_array[0].str);
                                printf("rxpk1.rssi: %d\n", rxpk_array[0].rssi);
                                printf("rxpk1.time: %s\n", rxpk_array[0].time);
#endif

                                    /* -------------------------------------------------------------------------- */
                                    /* --- STAGE : 当两个上行数据都错且crc值相同时进行纠错 ---------------------- */


                                    if (compareStat(rxpk_array, buffer_num)) {

                                        if (compareCRC(rxpk_array, buffer_num)){

                                            printf("/* ----------------------Error correction begins--------------------------------- */\n");

                                            CRCErrorNum++;
                                            PER = CRCErrorNum/(CRCErrorNum+NonCRCErrorNum);
                                            PDR = 1 - PER;
                                            printf("Packet error rate: %f\n", PER);
                                            printf("Packet delivery rate: %f\n", PDR);

                                            crc_get = rxpk_array[1].crc_get;

                                            /* -------------------------------------------------------------------------- */
                                            /* --- STAGE : Decoding ---------------------- */


                                            for(int loopcount=0; loopcount<=buffer_num-1; loopcount++){
                                                buffer_array[loopcount].payload[BUF_SIZE] = {0};
                                                memset(buffer_array[loopcount].payload, 0, BUF_SIZE*sizeof(uint8_t));

                                                buffer_array[loopcount].setSize(rxpk_array[loopcount].str); //与net_downlink相似，都是接收到data，故都用b64_to_bin
                                                cout<<"copy"<<loopcount<<" of data: "<<rxpk_array[loopcount].str<<endl;
                                            }


                                            uint16_t size;

                                            if (buffer_array[0].size == buffer_array[1].size) {
                                                size = buffer_array[0].size;
                                            }
                                            else {
                                                printf("Error: length1 is not equal to length2. This program will be shut down!");
                                                printf("/* ----------------------Error correction ends--------------------------------- */\n\n");
                                                continue;
                                            }

                                            int Hamming_weight_now = 0;
                                            getNe(buffer_array[0].payload, buffer_array[1].payload, size, Hamming_weight_now);

                                            /* -------------------------------------------------------------------------- */
                                            /* --- STAGE : uint8_t转char ---------------------- */ //https://bbs.csdn.net/topics/390141308

                                            for(int loopcount=0; loopcount<=buffer_num-1; loopcount++){
                                                buffer_array[loopcount].Hexstring = new char[BUF_SIZE];
                                                memset(buffer_array[loopcount].Hexstring, 0, BUF_SIZE * sizeof(char));

                                                buffer_array[loopcount].setHexstring();
                                            }


#if DEBUG
                                            printf("M's: %s\n", buffer_array[0].Hexstring);
                                            printf("M'r: %s\n", buffer_array[1].Hexstring);
#endif


                                            /* -------------------------------------------------------------------------- */
                                            /* --- STAGE : 十六进制字符串转二进制字符串 ---------------------- */ //https://blog.csdn.net/weixin_30279751/article/details/95437814

                                            for(int loopcount=0; loopcount<=buffer_num-1; loopcount++){
                                                buffer_array[loopcount].Binarystring = new char[BUF_SIZE];
                                                memset(buffer_array[loopcount].Binarystring, 0, BUF_SIZE * sizeof(char));

                                                buffer_array[loopcount].setBinarystring();
                                                delete[] buffer_array[loopcount].Hexstring;
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
                                            printf("Chosen copy: %s\n", rxpk_array[index].str);
#endif
                                            for(int loopcount=0; loopcount<=buffer_num-1; loopcount++){
                                                delete[] rxpk_array[loopcount].str;
                                            }

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
                                                printf("This program will be shut down!\n");
                                                printf("/* ----------------------Error correction ends--------------------------------- */\n\n");
                                                continue;

                                            }

#if DEBUG
                                            printf("Hamming_weight_now: %d\n", Hamming_weight_now);
#endif


                                            char* fakeresult = new char[BUF_SIZE]; //每次candidate与mch异或的中间产值
                                            memset(fakeresult, 0, BUF_SIZE * sizeof(char));

                                            char* realresult = new char[BUF_SIZE]; //符合CRC校验的fakeresult，但不保证能通过MIC校验
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
                                                printf("/* ----------------------Error correction ends--------------------------------- */\n\n");
                                                //CRC未出错的话一定出现了hidden error
                                                continue;
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
                                            memset(buffer.send_first_part_char, 0, BUF_SIZE * sizeof(char));

                                            buffer.send_last_part_char[BUF_SIZE] = { 0 };  //修改后的Upstream JSON data structure
                                            memset(buffer.send_last_part_char, 0, BUF_SIZE * sizeof(char));

                                            buffer.send = new uint8_t[BUF_SIZE];  //需要发送的数据 (原始uint8形式)
                                            memset(buffer.send, 0, BUF_SIZE * sizeof(uint8_t));


                                            /* -------------------------------------------------------------------------- */
                                            /* --- STAGE : 将Upstream JSON data structure的"data" field里面的数据使用修改后的data_up覆盖 ---------------------- */

                                            strncpy(buffer_array[index].inter + FindFirstSubchar(buffer_array[index].inter, "data") + 6, data_up, strlen(data_up)); //https://blog.csdn.net/zmhawk/article/details/44600075

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


#if DEBUG
                                            printf("buffer.send: ");
                                            for (int loopcount = 0; loopcount < buffer_array[index].index; loopcount++) {
                                                printf("%02X", buffer.send[count]);
                                            }
                                            printf("\n\n");

                                            printf("buffer.inter: %s\n", buffer.inter);
#endif
                                            printf("/* ----------------------Error correction ends--------------------------------- */\n\n");


                                            delete[] data_up;
                                            delete[] buffer.inter;
                                            delete[] buffer.inter_uint_char;


                                            /* -------------------------------------------------------------------------- */
                                            /* --- STAGE : 发送---------------------- */

                                            send(sock_up, (void*)buffer.send, buffer_array[index].index, 0);

                                        }
                                        else {
                                            printf("/* ----------------------Special case begins--------------------------------- */\n");
                                            NonCRCErrorNum++;
                                            PER = CRCErrorNum/(CRCErrorNum+NonCRCErrorNum);
                                            PDR = 1 - PER;
                                            printf("Packet error rate: %f\n", PER);
                                            printf("Packet delivery rate: %f\n", PDR);

                                            printf("Both two packets do not have the same FCS, no operation will be taken\n");

#if DEBUG
                                            for(int i=0; i<=buffer_num-1; i++){
                                            cout<<"buffer_send"<<i+1<<": ";
                                            for (int count = 0; count < buffer_array[i].index; count++) {
                                                printf("%02X", buffer_array[i].inter_uint[count]);
                                            }
                                            printf("\n\n");
                                            }
#endif

                                            /* -------------------------------------------------------------------------- */
                                            /* --- STAGE : 发送---------------------- */

                                            for(int loopcount = 0; loopcount <= buffer_num-1; loopcount++){
#if DEBUG
                                                cout<<"buffer"<<loopcount+1<<".inter: "<<buffer_array[loopcount].inter<<endl;
#endif
                                                send(sock_up, (void*)buffer_array[loopcount].inter_uint, buffer_array[loopcount].index, 0);
                                            }

                                            printf("/* ----------------------Special case ends--------------------------------- */\n\n");

                                            /* -------------------------------------------------------------------------- */
                                            /* --- STAGE : 以两者发送时重复一个rxinfo为代价换取case1能够执行（副作用是即使有两个数据，case1也会执行）---------------------- */

                                            for(int loopcount = 0; loopcount <= buffer_num-1; loopcount++){
                                                memset(buffer_array[loopcount].data, 0, BUF_SIZE * sizeof(char));
                                            }

                                        }

                                    }
                                    else {
                                        printf("/* ----------------------Special case begins--------------------------------- */\n");
                                        NonCRCErrorNum++;
                                        PER = CRCErrorNum/(CRCErrorNum+NonCRCErrorNum);
                                        PDR = 1 - PER;
                                        printf("Packet error rate: %f\n", PER);
                                        printf("Packet error rate: %f\n", PDR);

                                        printf("At least one packet is crc correct, no operation will be taken\n");

#if DEBUG
                                        for(int i=0; i<=buffer_num-1; i++){
                                        cout<<"buffer_send"<<i+1<<": ";
                                        for (int count = 0; count < buffer_array[i].index; count++) {
                                            printf("%02X", buffer_array[i].inter_uint[count]);
                                        }
                                        printf("\n\n");
                                       }
#endif


                                        /* -------------------------------------------------------------------------- */
                                        /* --- STAGE : 发送---------------------- */

                                        for(int loopcount = 0; loopcount <= buffer_num-1; loopcount++){
#if DEBUG
                                            cout<<"buffer"<<loopcount+1<<".inter: "<<buffer_array[loopcount].inter<<endl;
#endif
                                            send(sock_up, (void*)buffer_array[loopcount].inter_uint, buffer_array[loopcount].index, 0);
                                        }

                                        printf("/* ----------------------Special case ends--------------------------------- */\n\n");

                                        /* -------------------------------------------------------------------------- */
                                        /* --- STAGE : 以两者发送时重复一个rxinfo为代价换取case1能够执行（副作用是即使有两个数据，case1也会执行）---------------------- */

                                        for(int loopcount = 0; loopcount <= buffer_num-1; loopcount++){
                                            memset(buffer_array[loopcount].data, 0, BUF_SIZE * sizeof(char));
                                        }

                                    }

                                }
                            }
                                break;
                            case 1:{
                                if (buffer_array[0].index == 0 && buffer_array[1].index != 0) {
                                    printf("Only the %s gateway received message and transferd it\n\n", MAC_address2);
                                    send(sock_up, (void*)buffer_array[1].inter_uint, buffer_array[1].index, 0);

                                }
                                else if (buffer_array[0].index != 0 && buffer_array[1].index == 0) {
                                    printf("Only the %s gateway received message and transferd it\n\n", MAC_address1);
                                    send(sock_up, (void*)buffer_array[0].inter_uint, buffer_array[0].index, 0);

                                }
                            }
                                break;
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

