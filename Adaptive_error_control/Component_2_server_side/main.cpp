#include "header_2.h"

extern int sock_up;

extern char MAC_address1[];
extern char MAC_address2[];
extern char MAC_address3[];
extern char MAC_address4[];
extern int MAC_address_length;

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

    int buffer_num = 2;
    Buffer buffer_array[buffer_num];

    BufferSend buffer{};

    for(int loopcount=0; loopcount<=buffer_num-1; loopcount++){
        buffer_array[loopcount].data = new char[BUF_SIZE];
        memset(buffer_array[loopcount].data, 0, BUF_SIZE * sizeof(char));
    }

    int sfd, ss;
    int efd;
    struct epoll_event event;
    struct epoll_event* events;
    char* buf = new char[BUF_SIZE];

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

                    char* Gateway_unique_identifier = new char[MAC_address_length];
                    memset(Gateway_unique_identifier, 0, MAC_address_length * sizeof(char));
                    Gateway_unique_identifier[MAC_address_length]='\0';
                    strncpy(Gateway_unique_identifier, buf+MAC_address_length/2, MAC_address_length);
                    if(strcmp(Gateway_unique_identifier,MAC_address1)==0){
                        buffer_array[0].setData(buf);
                    }else if(strcmp(Gateway_unique_identifier,MAC_address2)==0){
                        buffer_array[1].setData(buf);
                    }

                    //printf("buffer1: %s\n", buffer1);

                    //printf("buffer2: %s\n", buffer2);

                    for(int loopcount=0; loopcount<=buffer_num-1; loopcount++){
                        buffer_array[loopcount].setIndex();
                        buffer_array[loopcount].uint[BUF_SIZE] = {0};
                        buffer_array[loopcount].setUint();
                    }


                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 对中间数据buffer_inter纠错---------------------- */

                    /*测试代码
                    printf("breakcount: %d\n\n", breakcount);
                    */

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 使用breakcount控制不发送重复数据---------------------- */

                    if (breakcount % 2 == 1) {

                        for(int loopcount=0; loopcount<=buffer_num-1; loopcount++){
                            buffer_array[loopcount].setInter(); //接收到的Upstream JSON data structure
                            //cout<<"buffer"<<i+1<<".inter: "<<buffer_array[i].inter<<endl;
                            buffer_array[loopcount].setInter_Uint();
                        }

                        /* -------------------------------------------------------------------------- */
                        /* --- STAGE : epoll的异步处理---------------------- */


                        Rxpk rxpk_array[buffer_num];

                        for(int loopcount=0; loopcount<=buffer_num-1; loopcount++)  rxpk_array[loopcount].setTime(buffer_array[loopcount].uint,buffer_array->buff_index);
                        ;

                        switch(countED(buffer_array, buffer_num)){
                            case 2:
                                if (compareTime(rxpk_array, buffer_num))
                                {
                                    send(sock_up, (void*)buffer_array[0].inter_uint, buffer_array[0].index, 0);
                                    send(sock_up, (void*)buffer_array[1].inter_uint, buffer_array[1].index, 0);

                                    /* -------------------------------------------------------------------------- */
                                    /* --- STAGE : 以两者发送时重复一个rxinfo为代价换取能够单独发送成功---------------------- */

                                    memset(buffer_array[0].data, 0, BUF_SIZE * sizeof(char));
                                    memset(buffer_array[1].data, 0, BUF_SIZE * sizeof(char));
                                };
                                break;
                            case 1:
                                if (buffer_array[0].index == 0 &&  buffer_array[1].index != 0){
                                    printf("Only the %s gateway received message and transferd it\n\n", MAC_address2);
                                    send(sock_up, (void*)buffer_array[1].inter_uint, buffer_array[1].index, 0);
                                }  else if (buffer_array[0].index != 0 && buffer_array[1].index == 0) {
                                    printf("Only the %s gateway received message and transferd it\n\n", MAC_address1);
                                    send(sock_up, (void*)buffer_array[0].inter_uint, buffer_array[0].index, 0);
                                }
                                break;
                            case 0:
                                cout<<0;
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

