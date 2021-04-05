//http://c.biancheng.net/socket/
//http://c.biancheng.net/view/2348.html
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define BUF_SIZE 140000


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
    char buffer[BUF_SIZE] = { 0 };

    while (1) {

        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

        //��ͻ��˷�������
        //char str[] = "http://c.biancheng.net/socket/";
        //write(clnt_sock, str, sizeof(str));


        //��ȡclient���ص�����
        recv(clnt_sock, buffer, sizeof(buffer) - 1, 0);
        printf("Message form client: %s\n", buffer);


        //�ر��׽���
        close(clnt_sock);
        memset(buffer, 0, BUF_SIZE);  //���û�����

    }

    close(serv_sock);

    return 0;
}
