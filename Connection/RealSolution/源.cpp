
//send: 假设已经接收到，将其纠错后发送

#include <stdint.h>         /* C99 types */
#include <stdbool.h>        /* bool type */
#include <stdio.h>          /* printf, fprintf, snprintf, fopen, fputs */
#include <inttypes.h>       /* PRIx64, PRIu64... */

#include <string.h>         /* memset */
#include <signal.h>         /* sigaction */
#include <time.h>           /* time, clock_gettime, strftime, gmtime */
#include <sys/time.h>       /* timeval */
#include <unistd.h>         /* getopt, access */
#include <stdlib.h>         /* atoi, exit */
#include <errno.h>          /* error messages */
#include <math.h>           /* modf */

#include <sys/socket.h>     /* socket specific definitions */
#include <netinet/in.h>     /* INET constants and stuff */
#include <arpa/inet.h>      /* IP address conversion stuff */
#include <netdb.h>          /* gai_strerror */

#include <pthread.h>
#define BUF_SIZE 140000

static char serv_addr[64] = "47.110.36.225";
static char serv_port_up[8] = "1700"; /* server port for upstream traffic */

static int sock_up; /* socket for upstream traffic */

void Char2Uint(char* array, uint8_t* array_uint, int length) {

    for (int count = 0; count < 2 * length; count++) {
        if (count % 2 == 0) {
            char buff_char[256] = { 0 };
            strncpy(buff_char, array + count, 2); //https://blog.csdn.net/zmhawk/article/details/44600075
            buff_char[strlen(buff_char)] = '\0';
            sscanf(buff_char, "%X", (int*)(&array_uint[count / 2])); //https://bbs.csdn.net/topics/391935459
        }
    }
}

void Uint2Char(uint8_t* array_uint, char* array, int length) {

    char buff[256] = "";

    for (uint16_t count = 0; count < length; count++) {

        sprintf(buff, "%02X", array_uint[count]);
        strcat(array, buff);

    }

}



int main() {

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 建立接收socket1 ---------------------- */

    //创建套接字
    int serv_sock1 = socket(AF_INET, SOCK_STREAM, 0);

    //将套接字和IP、端口绑定
    struct sockaddr_in serv_addr_receive1;
    memset(&serv_addr_receive1, 0, sizeof(serv_addr_receive1));  //每个字节都用0填充
    serv_addr_receive1.sin_family = AF_INET;  //使用IPv4地址
    serv_addr_receive1.sin_addr.s_addr = inet_addr("172.16.166.91");  //具体的IP地址
    serv_addr_receive1.sin_port = htons(1680);  //端口
    bind(serv_sock1, (struct sockaddr*)&serv_addr_receive1, sizeof(serv_addr_receive1));


    //进入监听状态，等待用户发起请求
    listen(serv_sock1, 20);

    //接收客户端请求
    struct sockaddr_in clnt_addr1;
    socklen_t clnt_addr_size1 = sizeof(clnt_addr1);

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 建立接收socket2 ---------------------- */

    //创建套接字
    int serv_sock2 = socket(AF_INET, SOCK_STREAM, 0);

    //将套接字和IP、端口绑定
    struct sockaddr_in serv_addr_receive2;
    memset(&serv_addr_receive2, 0, sizeof(serv_addr_receive2));  //每个字节都用0填充
    serv_addr_receive2.sin_family = AF_INET;  //使用IPv4地址
    serv_addr_receive2.sin_addr.s_addr = inet_addr("172.16.166.91");  //具体的IP地址
    serv_addr_receive2.sin_port = htons(1690);  //端口
    bind(serv_sock2, (struct sockaddr*)&serv_addr_receive2, sizeof(serv_addr_receive2));


    //进入监听状态，等待用户发起请求
    listen(serv_sock1, 20);


    //接收客户端请求
    struct sockaddr_in clnt_addr2;
    socklen_t clnt_addr_size2 = sizeof(clnt_addr2);

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
    /* --- STAGE : 处理数据 ---------------------- */


    char buffer1[BUF_SIZE] = { 0 };
    char buffer2[BUF_SIZE] = { 0 };

    while (1) {

        int clnt_sock1 = accept(serv_sock1, (struct sockaddr*)&clnt_addr1, &clnt_addr_size1);
        int clnt_sock2 = accept(serv_sock2, (struct sockaddr*)&clnt_addr2, &clnt_addr_size2);

        //读取client传回的数据
        recv(clnt_sock1, buffer1, sizeof(buffer1) - 1, 0);
        recv(clnt_sock2, buffer2, sizeof(buffer2) - 1, 0);

        //puts(buffer1);
        //puts(buffer2);

        int buff_index1 = strlen(buffer1) / 2;
        int buff_index2 = strlen(buffer2) / 2;

        uint8_t  buffer_uint1[BUF_SIZE] = "";
        uint8_t  buffer_uint2[BUF_SIZE] = "";
        Char2Uint(buffer1, buffer_uint1, buff_index1);
        Char2Uint(buffer2, buffer_uint2, buff_index2);
        char* buffer1_inter = (char*)(buffer_uint1 + 12);
        char* buffer2_inter = (char*)(buffer_uint2 + 12);
        uint8_t* buffer1_inter_uint = (uint8_t*)(buffer1_inter - 12);
        uint8_t* buffer2_inter_uint = (uint8_t*)(buffer2_inter - 12);

        send(sock_up, (void*)buffer1_inter_uint, buff_index1, 0);
        send(sock_up, (void*)buffer2_inter_uint, buff_index2, 0);


        //关闭套接字
        close(clnt_sock1);
        close(clnt_sock2);
        memset(buffer1, 0, BUF_SIZE);  //重置缓冲区
        memset(buffer2, 0, BUF_SIZE);  //重置缓冲区

    }


    close(serv_sock1);
    close(serv_sock2);
    close(sock_up);




    return 0;

}

