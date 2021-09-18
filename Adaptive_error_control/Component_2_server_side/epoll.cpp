#include"header_1_1.h"
#include"header_1_5.h"
#include "header_2.h"
#include "parson.h"

extern int sock_up;

int main() {

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : epoll特有的控制socket个数方法初始化 ---------------------- */

    int breakcount = 0;

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 建立发射socket ---------------------- */

    int i = create_up_socket();
    if (i==-1) abort();

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


                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 对中间数据buffer_inter纠错---------------------- */

                    /*测试代码
                    printf("breakcount: %d\n\n", breakcount);
                    */

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 使用breakcount控制不发送重复数据---------------------- */

                    if (breakcount % 2 == 1) {

                        char* buffer1_inter = (char*)(buffer_uint1 + 12);
                        char* buffer2_inter = (char*)(buffer_uint2 + 12);

                        //printf("buffer1_inter: %s\n", buffer1_inter);
                        //printf("\n");
                        //printf("buffer2_inter: %s\n", buffer2_inter);
                        //printf("\n");

                        uint8_t* buffer1_inter_uint = (uint8_t*)(buffer1_inter - 12);
                        uint8_t* buffer2_inter_uint = (uint8_t*)(buffer2_inter - 12);

                        /* -------------------------------------------------------------------------- */
                        /* --- STAGE : epoll的异步处理---------------------- */

                        Rxpk rxpk1;
                        Rxpk rxpk2;

                        rxpk1.time = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint1 + 12))), "rxpk"), 0), "time");
                        rxpk2.time = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint2 + 12))), "rxpk"), 0), "time");

                        if (buff_index1 != 0 && buff_index2 != 0) {

                            if (strcmp(rxpk1.time, rxpk2.time) == 0)
                            {
                                send(sock_up, (void*)buffer1_inter_uint, buff_index1, 0);
                                send(sock_up, (void*)buffer2_inter_uint, buff_index2, 0);

                                /* -------------------------------------------------------------------------- */
                                /* --- STAGE : 以两者发送时重复一个rxinfo为代价换取能够单独发送成功---------------------- */

                                memset(buffer1, 0, BUF_SIZE * sizeof(char));
                                memset(buffer2, 0, BUF_SIZE * sizeof(char));
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
                    /*测试代码
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

