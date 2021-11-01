#include"header_1_1.h"
#include"header_1_2.h"
#include"header_1_3.h"
#include "header_1_5.h"

#include "header_2_1.h"
#include "header_2_2.h"

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

    printf("The error control server waits for connections!\n\n");

    int i = create_up_socket();
    if (i==-1) abort();

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 开始处理数据 ---------------------- */

    int buffer_num = 4;
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
                        replaceData1(buff_up_char);
#endif
                        buffer_array[0].setData(buff_up_char);
                    }else if(strcmp(Gateway_unique_identifier,MAC_address2)==0){
#if DEBUG
                        replaceData2(buff_up_char);
#endif
                        buffer_array[1].setData(buff_up_char);
                    }else if(strcmp(Gateway_unique_identifier,MAC_address3)==0) {
#if DEBUG
                        replaceData3(buff_up_char);
#endif
                        buffer_array[2].setData(buff_up_char);
                    }else if(strcmp(Gateway_unique_identifier,MAC_address4)==0) {
#if DEBUG
                        replaceData4(buff_up_char);
#endif
                        buffer_array[3].setData(buff_up_char);
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
                        memset(buffer_array[loopcount].uint, 0, BUF_SIZE * sizeof(uint8_t));
                        buffer_array[loopcount].setUint();
                    }

#if DEBUG
                    printf("breakcount: %d\n\n", breakcount);
#endif

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 使用breakcount控制不发送重复数据---------------------- */


                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 对中间数据buffer_inter纠错---------------------- */
                    //TODO: false and true带来的多个包同时转发


                    for(int loopcount=0; loopcount<=buffer_num-1; loopcount++){
                        buffer_array[loopcount].setInter(); //接收到的Upstream JSON data structure
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
                        case 4:{

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

                                printf("buffer_send3: ");
                                for (int count = 0; count < buffer_array[2].index; count++) {
                                    printf("%02X", buffer_array[2].inter_uint[count]);
                                }
                                printf("\n");

                                printf("buffer_send4: ");
                                for (int count = 0; count < buffer_array[3].index; count++) {
                                    printf("%02X", buffer_array[3].inter_uint[count]);
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
                                /* --- STAGE : 当全部上行数据都错且crc值相同时进行纠错 ---------------------- */

                                if (compareDevAddr(rxpk_array, buffer_num)){ //avoid error=“get device-session error: object does not exist"

                                    if (compareStat(rxpk_array, buffer_num)) {

                                        if (compareCRC(rxpk_array, buffer_num)){}else{
                                            printf("/* ----------------------Special case begins--------------------------------- */\n");
                                            NonCRCErrorNum++;
                                            PER = CRCErrorNum/(CRCErrorNum+NonCRCErrorNum);
                                            PDR = 1 - PER;
                                            printf("Packet error rate: %f\n", PER);
                                            printf("Packet delivery rate: %f\n", PDR);

                                            printf("Not all packets have the same FCS, no operation will be taken\n");

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

                                        }
                                    } else {
                                        printf("/* ----------------------Special case begins--------------------------------- */\n");
                                        NonCRCErrorNum++;
                                        PER = CRCErrorNum/(CRCErrorNum+NonCRCErrorNum);
                                        PDR = 1 - PER;
                                        printf("Packet error rate: %f\n", PER);
                                        printf("Packet delivery rate: %f\n", PDR);

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

                                    }
                                }

                            }

                        }
                            break;
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

