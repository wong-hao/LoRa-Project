
//send: �����Ѿ����յ�������������

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

    //�����׽���
    int serv_sock = socket(AF_INET, SOCK_STREAM, 0);

    //���׽��ֺ�IP���˿ڰ�
    struct sockaddr_in serv_addr_receive;
    memset(&serv_addr_receive, 0, sizeof(serv_addr_receive));  //ÿ���ֽڶ���0���
    serv_addr_receive.sin_family = AF_INET;  //ʹ��IPv4��ַ
    serv_addr_receive.sin_addr.s_addr = inet_addr("172.16.166.91");  //�����IP��ַ
    //serv_addr_receive.sin_addr.s_addr = inet_addr("127.0.0.1");  //�����IP��ַ
    serv_addr_receive.sin_port = htons(1680);  //�˿�
    bind(serv_sock, (struct sockaddr*)&serv_addr_receive, sizeof(serv_addr_receive));


    //�������״̬���ȴ��û���������
    listen(serv_sock, 20);

    //���տͻ�������
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

    //��ͻ��˷�������
    //char str[] = "http://c.biancheng.net/socket/";
    //write(clnt_sock, str, sizeof(str));


    //��ȡclient���ص�����
    char buffer[140000];
    recv(clnt_sock, buffer, sizeof(buffer) - 1, 0);
    printf("Message form client: %s\n", buffer);
    char length_char[2555] = "";
    strcpy(length_char, buffer);
    puts(length_char);

    //�ر��׽���
    close(clnt_sock);
    close(serv_sock);

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


    int buff_index = strlen(length_char) + 12;
    printf("buff_index: %d\n", buff_index);


    uint8_t* buff_up_fake2 = (uint8_t*)(buffer - 12); //change data_up to buff_up_fake2��������str1_fullֻ�ǻ�û��ʼ�Ĵ�


    printf("buff_up_fake2: "); //�ճ�test_loragw_hal_rx��Ĵ�����ȷ�����͵�p->payload = PHYPayload
    for (int count = 0; count < buff_index; count++) {
        printf("%02X", buff_up_fake2[count]);
    }
    printf("\n");


    send(sock_up, (void*)buff_up_fake2, buff_index, 0); //socket send
    //printf("\nJSON up: %s", str1_full); /* DEBUG: display JSON payload */ //No need to print Json
    return 0;
}

