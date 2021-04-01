
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

int main() {



    /* network socket creation */ //socket�׽�������ͨ��
    struct addrinfo hints;
    struct addrinfo* result; /* store result of getaddrinfo */
    struct addrinfo* q; /* pointer to move into *result data */
    char host_name[64];
    char port_name[64];

    int i; /* loop variable and temporary variable for return value */


/* prepare hints to open network sockets */ //��ΪupstreamҲΪdownstream�����
    memset(&hints, 0, sizeof hints); //hints
    hints.ai_family = AF_INET; //IPV4 /* WA: Forcing IPv4 as AF_UNSPEC(IPV4 and IPV6) makes connection on localhost to fail */
    hints.ai_socktype = SOCK_DGRAM; //UDP
    //hints.ai_protocolȡĬ��ֵ0��ϵͳ���Զ����ݳ�Ӧ��ʹ��UDPЭ��

    /* look for server address w/ upstream port */
    i = getaddrinfo(serv_addr, serv_port_up, &hints, &result);
    //serv_addr��serv_port_up��parse_gateway_configuration�ó�����hints��ȡ��Ϣ�洢��result
    //��ΪIP + port -> socket
    if (i != 0) {
        printf("ERROR: [up] getaddrinfo on address %s (PORT %s) returned %s\n", serv_addr, serv_port_up, gai_strerror(i));
        exit(EXIT_FAILURE);
    }

    /* try to open socket for upstream traffic */
    for (q = result; q != NULL; q = q->ai_next) { //qָ��result��q�����Զ�������getaddrinfo�õ��ģ���Ϊһ���������ܲ�ֹһ��IP��ַ�����ԣ���Ҫ����res�е�next�����£��Ƿ�����һ���ڵ�
        sock_up = socket(q->ai_family, q->ai_socktype, q->ai_protocol); //�����׽���sock_up
        if (sock_up == -1) continue; /* try next field */
        else break; /* success, get out of loop */ //�õ�sock_up������forѭ����û�б�Ҫѭ������������q==NULL
    }
    if (q == NULL) { //һֱѭ�����˽�������q==NULL��û���sock_up
        printf("ERROR: [up] failed to open socket to any of server %s addresses (port %s)\n", serv_addr, serv_port_up);
        i = 1;
        for (q = result; q != NULL; q = q->ai_next) {
            getnameinfo(q->ai_addr, q->ai_addrlen, host_name, sizeof host_name, port_name, sizeof port_name, NI_NUMERICHOST);
            //��getaddrinfo������ת: socket -> IP + port
            printf("INFO: [up] result %i host:%s service:%s\n", i, host_name, port_name);
            ++i;
        }
        exit(EXIT_FAILURE); //�쳣�˳�����
    }

    /* connect so we can send/receive packet with the server only */
    i = connect(sock_up, q->ai_addr, q->ai_addrlen); //��������socket��qΪforѭ��break��ʱ��ֵ
    if (i != 0) {
        printf("ERROR: [up] connect returned %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(result); //�ͷŵ��ý�������ͨ��


    char Hexstring4[25600] = "02E78D000016C001FF10D3F67B227278706B223A5B7B226A766572223A312C22746D7374223A36363932383230312C2274696D65223A22323032312D30342D30315431313A32303A35352E3030303030303030305A222C22746D6D73223A313330313331313235353030302C226368616E223A312C2272666368223A302C2266726571223A3438362E3530303030302C226D6964223A20392C2273746174223A312C226D6F6475223A224C4F5241222C2264617472223A225346374257313235222C22636F6472223A22342F35222C227273736973223A2D392C226C736E72223A31332E352C22666F6666223A2D3237372C2272737369223A2D392C2273697A65223A31382C2264617461223A2251415154424361414177414368715967722B74347058704A227D5D7D"; //char���͵�PHYPayload

    uint8_t  Hexstring4_uint8[25600] = "";

    Char2Uint(Hexstring4, Hexstring4_uint8, 298);


    printf("PHYPayload: "); //�ճ�test_loragw_hal_rx��Ĵ�����ȷ�����͵�p->payload = PHYPayload
    for (int count = 0; count < 298; count++) {
        printf("%02X", Hexstring4_uint8[count]);
    }
    printf("\n");


    send(sock_up, (void*)Hexstring4_uint8, 298, 0); //socket send
    printf("\nJSON up: %s\n", (char*)(Hexstring4_uint8 + 12)); /* DEBUG: display JSON payload */
    return 0;
}
