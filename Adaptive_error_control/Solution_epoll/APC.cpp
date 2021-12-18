#include "header_1_1.h"
#include "header_1_2.h"
#include "header_1_3.h"
#include "header_1_5.h"

#include "header_2_1.h"
#include "header_2_2.h"

#include "parson.h"
#include "payload_crc.h"
#include "payload_diff.h"

#include "header_3.h"

#include "base64.h"
#include "timelib.h"

extern char serv_addr[];
extern char serv_port_up[];
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

    printf("Algorithm parameters: \n");
    printf("{\n    BUF_SIZE: %d\n", BUF_SIZE);
    printf("    MAXLATENCY: %f\n", MAXLATENCY);
    printf("    Hamming_weight_max: %d\n", Hamming_weight_max);
    printf("    StageOption: %d\n", StageOption);
    printf("    FakeOption: %d\n", FakeOption);

    if (Concurrent != 1) {
        printf("    Concurrent: %d (Should be '1'), the program will be shut down\n", Concurrent);
        printf("    DeprecatedOption: %d\n}\n", DeprecatedOption);
        return 0;
    }
    printf("    DeprecatedOption: %d\n}\n", DeprecatedOption);
    printf("The error control server (port: %d) waits for connections and forward to Network server (address: %s, port: %s)!\n", ser_port, serv_addr, serv_port_up);

    int i = create_up_socket();
    if (i == -1) abort();

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 开始处理数据 ---------------------- */

    int buffer_num = 4;
    Buffer buffer_array[buffer_num];

    BufferSend buffer{};

    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
        buffer_array[loopcount].data = new char[BUF_SIZE];
        memset(buffer_array[loopcount].data, 0, BUF_SIZE * sizeof(char));
    }

    double throughoutData = 0;//PHY Payload
    double throughout = 0;

    double CRCErrorNumBefore = 0;
    double NonCRCErrorNumBefore = 0;
    double PERBefore;
    double PDRBefore;

    bool CorrectedFlag = false; //防止纠错不成功后仍使得NonCRCErrorNumAfter错误增加
    double CRCErrorNumAfter = 0;//计算无论经过纠错或未经过，最终未通过CRC校验的次数
    double NonCRCErrorNumAfter = 0;
    double PERAfter;//计算无论经过纠错或未经过，最终未通过CRC校验的全局PER
    double PDRAfter;

    int sfd, ss;
    int efd;
    struct epoll_event event;
    struct epoll_event *events;
    char *buff_up_char = new char[BUF_SIZE];

    sfd = create_and_bind();
    if (sfd == -1)
        abort();

    ss = make_socket_non_blocking(sfd);
    if (ss == -1)
        abort();

    ss = listen(sfd, SOMAXCONN);
    if (ss == -1) {
        perror("listen");
        abort();
    }

    efd = epoll_create1(0);
    if (efd == -1) {
        perror("epoll_create");
        abort();
    }

    event.data.fd = sfd;
    event.events = EPOLLIN | EPOLLET;
    ss = epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &event);
    if (ss == -1) {
        perror("epoll_ctl");
        abort();
    }

    /* Buffer where events are returned */
    //events = calloc(MAXEVENTS, sizeof event);
    events = new epoll_event[MAXEVENTS];
    memset(events, 0, MAXEVENTS * sizeof(epoll_event));

    /* The event loop */
    while (1) {
        int n, i;

        n = epoll_wait(efd, events, MAXEVENTS, -1);
        for (i = 0; i < n; i++) {
            if ((events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP) ||
                (!(events[i].events & EPOLLIN))) {
                /* An error has occured on this fd, or the socket is not
                         ready for reading (why were we notified then?) */
                fprintf(stderr, "epoll error\n");
                close(events[i].data.fd);
                continue;
            }

            else if (sfd == events[i].data.fd) {
                /* We have a notification on the listening socket, which
                         means one or more incoming connections. */
                while (1) {
                    struct sockaddr in_addr;
                    socklen_t in_len;
                    int infd;
                    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

                    in_len = sizeof in_addr;
                    infd = accept(sfd, &in_addr, &in_len);
                    if (infd == -1) {
                        if ((errno == EAGAIN) ||
                            (errno == EWOULDBLOCK)) {
                            /* We have processed all incoming
                                           connections. */
                            break;
                        } else {
                            perror("accept");
                            break;
                        }
                    }

                    ss = getnameinfo(&in_addr, in_len,
                                     hbuf, sizeof hbuf,
                                     sbuf, sizeof sbuf,
                                     NI_NUMERICHOST | NI_NUMERICSERV);
                    if (ss == 0) {
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
                    if (ss == -1) {
                        perror("epoll_ctl");
                        abort();
                    }
                }
                continue;
            } else {
                /* We have data on the fd waiting to be read. Read and
                         display it. We must read whatever data is available
                         completely, as we are running in edge-triggered mode
                         and won't get a notification again for the same
                         data. */
                int done = 0;

                while (1) {
                    breakcount++;
                    ssize_t count;
                    memset(buff_up_char, 0, BUF_SIZE * sizeof(char));
                    count = read(events[i].data.fd, buff_up_char, BUF_SIZE * sizeof buff_up_char);

                    char *Gateway_unique_identifier = new char[MAC_address_length];
                    memset(Gateway_unique_identifier, 0, MAC_address_length * sizeof(char));
                    Gateway_unique_identifier[MAC_address_length] = '\0';
                    strncpy(Gateway_unique_identifier, buff_up_char + MAC_address_length / 2, MAC_address_length);
                    if (strcmp(Gateway_unique_identifier, MAC_address1) == 0) {
#if FakeOption
                        replaceData1(buff_up_char);
#endif
                        buffer_array[0].setData(buff_up_char);
                    } else if (strcmp(Gateway_unique_identifier, MAC_address2) == 0) {
#if FakeOption
                        replaceData2(buff_up_char);
#endif
                        buffer_array[1].setData(buff_up_char);
                    } else if (strcmp(Gateway_unique_identifier, MAC_address3) == 0) {
#if FakeOption
                        replaceData3(buff_up_char);
#endif
                        buffer_array[2].setData(buff_up_char);
                    } else if (strcmp(Gateway_unique_identifier, MAC_address4) == 0) {
#if FakeOption
                        replaceData4(buff_up_char);
#endif
                        buffer_array[3].setData(buff_up_char);
                    }

#if FakeOption
                    //TODO: 检查socketexample初始时单个网关是否会导致多接收/以及这里的buffer是否，若是则需要换框架
                    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                        cout << "buffer" << loopcount + 1 << ": " << buffer_array[loopcount].data << endl;
                    }

                    printf("\n");
#endif

                    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                        buffer_array[loopcount].setIndex();
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


                    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                        buffer_array[loopcount].setInter();//接收到的Upstream JSON data structure
                        memset(buffer_array[loopcount].inter, 0, BUF_SIZE * sizeof(char));
                        buffer_array[loopcount].setInter();//接收到的Upstream JSON data structure
#if DEBUG
                        cout << "buffer" << loopcount + 1 << ".inter: " << buffer_array[loopcount].inter << endl;
#endif
                        buffer_array[loopcount].inter_uint = new uint8_t[BUF_SIZE];
                        memset(buffer_array[loopcount].inter_uint, 0, BUF_SIZE * sizeof(uint8_t));
                        buffer_array[loopcount].setInter_Uint();
                    }

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : epoll的异步处理---------------------- */

                    Rxpk rxpk_array[buffer_num];

                    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                        rxpk_array[loopcount].setTime(buffer_array[loopcount].uint, buffer_array->buff_index);
                        rxpk_array[loopcount].setFcnt(buffer_array[loopcount].uint, buffer_array->buff_index);
                    }

                    switch (countED(buffer_array, buffer_num)) {
                        case 4: {

                            if (compareTime(rxpk_array, buffer_num) || compareFcnt(rxpk_array, buffer_num)) {

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
                                    cout << "buffer" << loopcount + 1 << ".inter: " << buffer_array[loopcount].inter << endl;
                                    send(sock_up, (void *) buffer_array[loopcount].inter_uint, buffer_array[loopcount].index, 0);
                                }

                                printf("\n");


                                /* -------------------------------------------------------------------------- */
                                /* --- STAGE : 找到上行数据中需要的属性的值 ---------------------- */
                                //TODO: 解决多数据包同时上行情况 (重复数据包接收是因为距离太近)
                                //https://forum.rakwireless.com/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/3980/3?u=haowong
                                //https://forum.chirpstack.io/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/10886/2?u=shirou_emiya

                                for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                                    rxpk_array[loopcount].setDevAddr_get(buffer_array[loopcount].uint, buffer_array->buff_index);
                                    rxpk_array[loopcount].setTime(buffer_array[loopcount].uint, buffer_array->buff_index);
                                    rxpk_array[loopcount].setFcnt(buffer_array[loopcount].uint, buffer_array->buff_index);
                                    rxpk_array[loopcount].setStat(buffer_array[loopcount].uint, buffer_array->buff_index);
                                    rxpk_array[loopcount].setCrc_get(buffer_array[loopcount].uint, buffer_array->buff_index);
                                    rxpk_array[loopcount].setStr(buffer_array[loopcount].uint, buffer_array->buff_index);
                                    rxpk_array[loopcount].setRssi(buffer_array[loopcount].uint, buffer_array->buff_index);
                                    rxpk_array[loopcount].setSNR(buffer_array[loopcount].uint, buffer_array->buff_index);
                                    rxpk_array[loopcount].setPayloadSize(buffer_array[loopcount].uint, buffer_array->buff_index);

#if DEBUG
                                    printf("\nrxpk%d.DevAddr_get: %d\n", loopcount + 1, rxpk_array[loopcount].DevAddr_get);
                                    printf("rxpk%d.stat: %d\n", loopcount + 1, rxpk_array[loopcount].stat);
                                    printf("rxpk%d.crc_get: %d\n", loopcount + 1, rxpk_array[loopcount].crc_get);
                                    printf("rxpk%d.str: %s\n", loopcount + 1, rxpk_array[loopcount].str);
                                    printf("rxpk%d.rssi: %d\n", loopcount + 1, rxpk_array[loopcount].rssi);
                                    printf("rxpk%d.snr: %f\n", loopcount + 1, rxpk_array[loopcount].snr);
                                    printf("rxpk%d.time: %s\n", loopcount + 1, rxpk_array[loopcount].time);
                                    printf("rxpk%d.fcnt: %d\n", loopcount + 1, rxpk_array[0].fcnt);
                                    printf("rxpk%d.PayloadSize: %d\n\n", loopcount + 1, rxpk_array[0].PayloadSize);
#endif

                                    rxpk_array[loopcount].crc = new char[BUF_SIZE];
                                    memset(rxpk_array[loopcount].crc, 0, BUF_SIZE * sizeof(char));

                                    sprintf(rxpk_array[loopcount].crc, "0x%04X", rxpk_array[loopcount].crc_get);
#if DEBUG
                                    printf("Processed CRC%d: %s\n", loopcount + 1, rxpk_array[loopcount].crc);
#endif
                                    char *ptr;
                                    rxpk_array[loopcount].crc_hex = strtoul(rxpk_array[loopcount].crc, &ptr, 16);
#if DEBUG
                                    printf("Processed CRC%d: 0x%04X\n", loopcount + 1, rxpk_array[loopcount].crc_hex);
#endif

                                    rxpk_array[loopcount].DevAddr = new char[BUF_SIZE];
                                    memset(rxpk_array[loopcount].DevAddr, 0, BUF_SIZE * sizeof(char));

                                    sprintf(rxpk_array[loopcount].DevAddr, "0x%08X", rxpk_array[loopcount].DevAddr_get);
#if DEBUG
                                    printf("Processed DevAddr%d: %s\n", loopcount + 1, rxpk_array[loopcount].DevAddr);
#endif
                                    rxpk_array[loopcount].DevAddr_hex = strtoul(rxpk_array[loopcount].DevAddr, &ptr, 16);
#if DEBUG
                                    printf("Processed DevAddr%d: 0x%08X\n", loopcount + 1, rxpk_array[loopcount].DevAddr_hex);
#endif
                                }

                            /* -------------------------------------------------------------------------- */
                            /* --- STAGE : 当全部上行数据都错且crc值相同时进行纠错 ---------------------- */

                        } break;
                    }


                    if (count == -1) {
                        /* If errno == EAGAIN, that means we have read all
                                     data. So go back to the main loop. */
                        if (errno != EAGAIN) {
                            perror("read");
                            done = 1;
                        }
                        break;
                    } else if (count == 0) {
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

                if (done) {
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
