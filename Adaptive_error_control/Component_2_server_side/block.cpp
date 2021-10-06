#include"header_1_1.h"
#include"header_1_2.h"
#include"header_1_3.h"
#include"header_1_7.h"
#include "header_2.h"

extern int sock_up;

int main() {

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 建立接收socket1 ---------------------- */

    //创建套接字
    int serv_sock1 = socket(AF_INET, SOCK_STREAM, 0);

    // 设置套接字选项避免地址使用错误 (https://www.cnblogs.com/argenbarbie/p/4118783.html)
    int on=1;
    if((setsockopt(serv_sock1,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)
    {
        perror("setsockopt failed");

        return -1;
    }

    //将套接字和IP、端口绑定
    struct sockaddr_in serv_addr_receive1;
    memset(&serv_addr_receive1, 0, sizeof(serv_addr_receive1));  //每个字节都用0填充
    serv_addr_receive1.sin_family = AF_INET;  //使用IPv4地址
    serv_addr_receive1.sin_addr.s_addr = INADDR_ANY;
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
    serv_addr_receive2.sin_addr.s_addr = INADDR_ANY;
    serv_addr_receive2.sin_port = htons(1690); //端口
    bind(serv_sock2, (struct sockaddr*)&serv_addr_receive2, sizeof(serv_addr_receive2));


    //进入监听状态，等待用户发起请求
    listen(serv_sock2, 20);


    //接收客户端请求
    struct sockaddr_in clnt_addr2;
    socklen_t clnt_addr_size2 = sizeof(clnt_addr2);

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 建立发射socket ---------------------- */

    int i = create_up_socket();
    if (i==-1) abort();

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 开始处理数据 ---------------------- */


    char buffer1[BUF_SIZE] = { 0 };
    char buffer2[BUF_SIZE] = { 0 };

    while (1) {

        int clnt_sock1 = accept(serv_sock1, (struct sockaddr*)&clnt_addr1, &clnt_addr_size1);
        //TODO: 解决掉由于只有一个gateway成功连接导致的accept阻塞问题
        int clnt_sock2 = accept(serv_sock2, (struct sockaddr*)&clnt_addr2, &clnt_addr_size2);
    	
        //读取client传回的数据
        recv(clnt_sock1, buffer1, sizeof(buffer1) - 1, 0);
        recv(clnt_sock2, buffer2, sizeof(buffer2) - 1, 0);

        /*测试代码
        printf("buffer1: %s\n", buffer1);
        printf("\n");
        printf("buffer2: %s\n", buffer2);
        printf("\n");
        */

        int buff_index1 = strlen(buffer1) / 2;
        int buff_index2 = strlen(buffer2) / 2;

        uint8_t  buffer_uint1[BUF_SIZE] = { 0 };
        uint8_t  buffer_uint2[BUF_SIZE] = { 0 };
        Char2Uint(buffer1, buffer_uint1);
        Char2Uint(buffer2, buffer_uint2);

        
        /* -------------------------------------------------------------------------- */
        /* --- STAGE : 对中间数据buffer_inter纠错---------------------- */

        char* buffer1_inter = (char*)(buffer_uint1 + 12);
        char* buffer2_inter = (char*)(buffer_uint2 + 12);
        printf("buffer1_inter: %s\n", buffer1_inter);
        printf("\n");
        printf("buffer2_inter: %s\n", buffer2_inter);
        printf("\n");

        uint8_t* buffer1_inter_uint = (uint8_t*)(buffer1_inter - 12);
        uint8_t* buffer2_inter_uint = (uint8_t*)(buffer2_inter - 12);


        /* -------------------------------------------------------------------------- */
        /* --- STAGE : 发送---------------------- */


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

