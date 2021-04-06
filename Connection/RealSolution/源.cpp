#include"header.h"

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

int FindSubchar(char* fullchar, char* subchar) {

    char* buffer; //���ڽ��ܷ���ֵ

    if ((buffer = strstr(fullchar, subchar)) == NULL)
    { //˵��û��Ҫ�ҵ��ַ���
        return -1;
    }
    else
    {                                 //˵���ҵ����Ǹ��ַ���
        return buffer - fullchar + 1; //cde�ĵ�ַ��ȥabcde�ĵ�ַ+1
    }

}


int main() {

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : ��������socket1 ---------------------- */

    //�����׽���
    int serv_sock1 = socket(AF_INET, SOCK_STREAM, 0);

    //���׽��ֺ�IP���˿ڰ�
    struct sockaddr_in serv_addr_receive1;
    memset(&serv_addr_receive1, 0, sizeof(serv_addr_receive1));  //ÿ���ֽڶ���0���
    serv_addr_receive1.sin_family = AF_INET;  //ʹ��IPv4��ַ
    serv_addr_receive1.sin_addr.s_addr = inet_addr("172.16.166.91");  //�����IP��ַ
    serv_addr_receive1.sin_port = htons(1680);  //�˿�
    bind(serv_sock1, (struct sockaddr*)&serv_addr_receive1, sizeof(serv_addr_receive1));


    //�������״̬���ȴ��û���������
    listen(serv_sock1, 20);

    //���տͻ�������
    struct sockaddr_in clnt_addr1;
    socklen_t clnt_addr_size1 = sizeof(clnt_addr1);

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : ��������socket2 ---------------------- */

    //�����׽���
    int serv_sock2 = socket(AF_INET, SOCK_STREAM, 0);

    //���׽��ֺ�IP���˿ڰ�
    struct sockaddr_in serv_addr_receive2;
    memset(&serv_addr_receive2, 0, sizeof(serv_addr_receive2));  //ÿ���ֽڶ���0���
    serv_addr_receive2.sin_family = AF_INET;  //ʹ��IPv4��ַ
    serv_addr_receive2.sin_addr.s_addr = inet_addr("172.16.166.91");  //�����IP��ַ
    serv_addr_receive2.sin_port = htons(1690); //�˿�
    bind(serv_sock2, (struct sockaddr*)&serv_addr_receive2, sizeof(serv_addr_receive2));


    //�������״̬���ȴ��û���������
    listen(serv_sock2, 20);


    //���տͻ�������
    struct sockaddr_in clnt_addr2;
    socklen_t clnt_addr_size2 = sizeof(clnt_addr2);

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : ��������socket ---------------------- */

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

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : ��ʼ�������� ---------------------- */


    char buffer1[BUF_SIZE] = { 0 };
    char buffer2[BUF_SIZE] = { 0 };

    while (1) {

        int clnt_sock1 = accept(serv_sock1, (struct sockaddr*)&clnt_addr1, &clnt_addr_size1);
        //TODO: ���������ֻ��һ��gateway�ɹ����ӵ��µ�accept��������
        int clnt_sock2 = accept(serv_sock2, (struct sockaddr*)&clnt_addr2, &clnt_addr_size2);

        //��ȡclient���ص�����
        recv(clnt_sock1, buffer1, sizeof(buffer1) - 1, 0);
        recv(clnt_sock2, buffer2, sizeof(buffer2) - 1, 0);

        /*���Դ���
        printf("buffer1: %s\n", buffer1);
        printf("\n");
        printf("buffer2: %s\n", buffer2);
        printf("\n");
        */

        int buff_index1 = strlen(buffer1) / 2;
        int buff_index2 = strlen(buffer2) / 2;

        uint8_t  buffer_uint1[BUF_SIZE] = "";
        uint8_t  buffer_uint2[BUF_SIZE] = "";
        Char2Uint(buffer1, buffer_uint1, buff_index1);
        Char2Uint(buffer2, buffer_uint2, buff_index2);

        char* buffer1_inter = (char*)(buffer_uint1 + 12);
        char* buffer2_inter = (char*)(buffer_uint2 + 12);
        printf("buffer1_inter: %s\n", buffer1_inter);
        printf("\n");
        printf("buffer2_inter: %s\n", buffer2_inter);
        printf("\n");

        /* -------------------------------------------------------------------------- */
        /* --- STAGE : ��������---------------------- */
        //TODO: false and true�����Ķ����ͬʱת������buffer1��buffer2���и���rssi����������������

        send(sock_up, (void*)buffer_uint1, buff_index1, 0);
        send(sock_up, (void*)buffer_uint2, buff_index2, 0);


        //�ر��׽���
        close(clnt_sock1);
        close(clnt_sock2);
        memset(buffer1, 0, BUF_SIZE);  //���û�����
        memset(buffer2, 0, BUF_SIZE);  //���û�����

    }


    close(serv_sock1);
    close(serv_sock2);
    close(sock_up);




    return 0;

}

