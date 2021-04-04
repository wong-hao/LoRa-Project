//http://c.biancheng.net/socket/
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

char serv_addr[64] = "172.16.167.190"; /* address of the server (host name or IPv4/IPv6) */
char serv_port_up[8] = "1680"; /* server port for upstream traffic */
static int sock_inter;

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


    //创建套接字
    sock_inter = socket(AF_INET, SOCK_STREAM, 0);
    int value;

    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addrs;
    memset(&serv_addrs, 0, sizeof(serv_addrs));  //每个字节都用0填充
    serv_addrs.sin_family = AF_INET;  //使用IPv4地址
    serv_addrs.sin_addr.s_addr = inet_addr(serv_addr);  //具体的IP地址
    serv_addrs.sin_port = htons(atoi(serv_port_up));  //端口
    value = connect(sock_inter, (struct sockaddr*)&serv_addrs, sizeof(serv_addrs));
    if (value != 0) {
        printf("ERROR: [up] connect returned %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    //读取服务器传回的数据
    //char buffer[40];
    //read(sock, buffer, sizeof(buffer)-1);

    //printf("Message form server: %s\n", buffer);

    //向server发送数据
    //char str[] = "Cliensts is here";
    //send(sock_inter, str, sizeof(str), 0);

    char buff_up_fake_char[25600] = "02E78D000016C001FF10D3F67B227278706B223A5B7B226A766572223A312C22746D7374223A36363932383230312C2274696D65223A22323032312D30342D30315431313A32303A35352E3030303030303030305A222C22746D6D73223A313330313331313235353030302C226368616E223A312C2272666368223A302C2266726571223A3438362E3530303030302C226D6964223A20392C2273746174223A312C226D6F6475223A224C4F5241222C2264617472223A225346374257313235222C22636F6472223A22342F35222C227273736973223A2D392C226C736E72223A31332E352C22666F6666223A2D3237372C2272737369223A2D392C2273697A65223A31382C2264617461223A2251415154424361414177414368715967722B74347058704A227D5D7D"; //char类型的PHYPayload
    uint8_t  buff_up_fake[25600] = ""; //received
    int buff_index = 298;
    Char2Uint(buff_up_fake_char, buff_up_fake, buff_index);  //To receive buff_up_fake
    char buff_up_fake_inter[25600]; //把接收到的数据化成char发送给另一个树莓派
    Uint2Char(buff_up_fake, buff_up_fake_inter, buff_index);
    send(sock_inter, buff_up_fake_inter, sizeof(buff_up_fake_inter), 0);
    //send(sock_inter, (void*)buff_up_fake, buff_index, 0); //socket send



    //关闭套接字
    close(sock_inter);

    return 0;
}
