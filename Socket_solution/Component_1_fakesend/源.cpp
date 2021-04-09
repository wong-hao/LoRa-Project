
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

void Stream2Json(char* array1, uint8_t* array2) {

    //puts((char*)(array2 + 12));
    //for(int 
    //strcpy(array1,(char*)(array2 + 12));
}


int main() {



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


    char buffer[25600] = "026973000016C001FF10D3F67B227278706B223A5B7B226A766572223A312C22746D7374223A323733383430352C2274696D65223A22323032312D30342D30345431313A32363A34312E3030303030303030305A222C22746D6D73223A313330313537303830313030302C226368616E223A322C2272666368223A302C2266726571223A3438362E3730303030302C226D6964223A20382C2273746174223A312C226D6F6475223A224C4F5241222C2264617472223A225346374257313235222C22636F6472223A22342F35222C227273736973223A2D31362C226C736E72223A31332E382C22666F6666223A2D3234322C2272737369223A2D31362C2273697A65223A31382C2264617461223A225141515442436141434141437A614D53757046414A546B61227D5D7D"; //char类型的PHYPayload

    int buff_index = strlen(buffer) / 2;

    uint8_t  buffer_uint[25600] = "";
    Char2Uint(buffer, buffer_uint, buff_index);
    char* buffer2 = (char*)(buffer_uint + 12);
    //puts(buffer2);
    uint8_t* buffer2_uint = (uint8_t*)(buffer2 - 12);


    send(sock_up, (void*)buffer2_uint, buff_index, 0);
    return 0;
}

