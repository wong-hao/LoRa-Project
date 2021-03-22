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

char serv_addr[64] = "127.0.0.1"; /* address of the server (host name or IPv4/IPv6) */
char serv_port_up[8] = "1700"; /* server port for upstream traffic */
static int sock_up;

int main() {


    //�����׽���
    sock_up = socket(AF_INET, SOCK_STREAM, 0);
    int i;

    //����������ض���IP�Ͷ˿ڣ���������
    struct sockaddr_in serv_addrs;
    memset(&serv_addrs, 0, sizeof(serv_addrs));  //ÿ���ֽڶ���0���
    serv_addrs.sin_family = AF_INET;  //ʹ��IPv4��ַ
    serv_addrs.sin_addr.s_addr = inet_addr(serv_addr);  //�����IP��ַ
    serv_addrs.sin_port = htons(atoi(serv_port_up));  //�˿�
    i = connect(sock_up, (struct sockaddr*)&serv_addrs, sizeof(serv_addrs));
    if (i != 0) {
        printf("ERROR: [up] connect returned %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    //��ȡ���������ص�����
    //char buffer[40];
    //read(sock, buffer, sizeof(buffer)-1);

    //printf("Message form server: %s\n", buffer);

    //��server��������
    uint8_t str[] = "Cliensts is here";

    send(sock_up, str, sizeof(str), 0);

    //�ر��׽���
    close(sock_up);

    return 0;
}
