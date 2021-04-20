//http://c.biancheng.net/socket/
//http://c.biancheng.net/view/2348.html
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define BUF_SIZE 14400


int main() {
    //创建套接字
    int serv_sock = socket(AF_INET, SOCK_STREAM, 0);

    //将套接字和IP、端口绑定
    struct sockaddr_in serv_addr_receive;
    memset(&serv_addr_receive, 0, sizeof(serv_addr_receive));  //每个字节都用0填充
    serv_addr_receive.sin_family = AF_INET;  //使用IPv4地址
    serv_addr_receive.sin_addr.s_addr = INADDR_ANY;
    //serv_addr_receive.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr_receive.sin_port = htons(1680);  //端口
    bind(serv_sock, (struct sockaddr*)&serv_addr_receive, sizeof(serv_addr_receive));


    //进入监听状态，等待用户发起请求
    listen(serv_sock, 20);

    //接收客户端请求
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    char buffer[BUF_SIZE] = { 0 };

    while (1) {

        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

        //向客户端发送数据
        //char str[] = "http://c.biancheng.net/socket/";
        //write(clnt_sock, str, sizeof(str));


        //读取client传回的数据
        recv(clnt_sock, buffer, sizeof(buffer) - 1, 0);
        printf("Message form client: %s\n", buffer);


        //关闭套接字
        close(clnt_sock);
        memset(buffer, 0, BUF_SIZE);  //重置缓冲区

    }

    close(serv_sock);

    return 0;
}
