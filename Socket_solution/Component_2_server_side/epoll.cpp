#include"header_1_1.h"
#include"header_1_2.h"
#include"header_1_3.h"
#include"header_1_5.h"
#include "parson.h"


#include"header_2_2.h"

static int
make_socket_non_blocking(int sfd)
{
    int flags, ss;

    flags = fcntl(sfd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl");
        return -1;
    }

    flags |= O_NONBLOCK;
    ss = fcntl(sfd, F_SETFL, flags);
    if (ss == -1)
    {
        perror("fcntl");
        return -1;
    }

    return 0;
}

static int
create_and_bind()
{
    struct addrinfo hints;
    struct addrinfo* result, * rp;
    int ss, sfd;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
    hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
    hints.ai_flags = AI_PASSIVE;     /* All interfaces */
    char port[8] = "1680"; /* server port for upstream traffic */

    ss = getaddrinfo(NULL, port, &hints, &result);
    if (ss != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ss));
        return -1;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;

        ss = bind(sfd, rp->ai_addr, rp->ai_addrlen);
        if (ss == 0)
        {
            /* We managed to bind successfully! */
            break;
        }

        close(sfd);
    }

    if (rp == NULL)
    {
        fprintf(stderr, "Could not bind\n");
        return -1;
    }

    freeaddrinfo(result);

    return sfd;
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
    char* deduplicate1 = new char[BUF_SIZE];
    char* deduplicate2 = new char[BUF_SIZE];

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


                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 对中间数据buffer_inter纠错---------------------- */
                    //TODO: false and true带来的多个包同时转发；根据rssi纠错（必须两个都错以降低时间复杂度）；判断纠错的两个包的crc值是否相同

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

