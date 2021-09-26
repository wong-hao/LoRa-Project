//https://www.cnblogs.com/wuyepeng/p/9726771.html

#include"header_1_1.h"
#include"header_1_2.h"
#include"header_1_3.h"
#include "payload_crc.h"
#include "header_2.h"
#include"header_1_6.h"
#include "parson.h"

#include"payload_diff.h"

#include"header_3.h"

#include "base64.h"

extern int sock_up;

extern char MAC_address1[];
extern char MAC_address2[];
extern char MAC_address3[];
extern char MAC_address4[];
extern int MAC_address_length;


int main() {


    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 建立发射socket ---------------------- */

    int i = create_up_socket();
    if (i==-1) abort();

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 开始处理数据 ---------------------- */

    int buffer_num = 4;
    Buffer buffer_array[buffer_num];

    BufferSend buffer{};

    for(int loopcount=0; loopcount<=buffer_num-1; loopcount++){
        buffer_array[loopcount].data = new char[BUF_SIZE];
        memset(buffer_array[loopcount].data, 0, BUF_SIZE * sizeof(char));
    }

    int ser_souck_fd;

    char input_message[BUF_SIZE] = {0};
    char* buff_up_char = new char[BUF_SIZE];

    struct sockaddr_in ser_addr;
    ser_addr.sin_family = AF_INET;    //IPV4
    ser_addr.sin_port = htons(ser_port);
    ser_addr.sin_addr.s_addr = INADDR_ANY;  //指定的是所有地址

    //creat socket
    if ((ser_souck_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("creat failure");
        return -1;
    }

    //bind soucket
    if (bind(ser_souck_fd, (const struct sockaddr*)&ser_addr, sizeof(ser_addr)) < 0)
    {
        perror("bind failure");
        return -1;
    }

    //listen
    if (listen(ser_souck_fd, backlog) < 0)
    {
        perror("listen failure");
        return -1;
    }


    //fd_set
    fd_set ser_fdset;
    int max_fd = 1;
    struct timeval mytime;
    //printf("wait for client connnect!\n");

    while (1)
    {
        mytime.tv_sec = 27;
        mytime.tv_usec = 0;

        FD_ZERO(&ser_fdset);

        //add standard input
        FD_SET(0, &ser_fdset);
        if (max_fd < 0)
        {
            max_fd = 0;
        }

        //add serverce
        FD_SET(ser_souck_fd, &ser_fdset);
        if (max_fd < ser_souck_fd)
        {
            max_fd = ser_souck_fd;
        }

        //add client
        for (int i = 0; i < CLI_NUM; i++)  //用数组定义多个客户端fd
        {
            if (client_fds[i] != 0)
            {
                FD_SET(client_fds[i], &ser_fdset);
                if (max_fd < client_fds[i])
                {
                    max_fd = client_fds[i];
                }
            }
        }

        //select多路复用
        int ret = select(max_fd + 1, &ser_fdset, NULL, NULL, &mytime);

        if (ret < 0)
        {
            perror("select failure\n");
            continue;
        }

        else if (ret == 0)
        {
            printf("time out!");
            continue;
        }

        else
        {
            if (FD_ISSET(0, &ser_fdset)) //标准输入是否存在于ser_fdset集合中（也就是说，检测到输入时，做如下事情）
            {
                printf("send message to");
                memset(input_message, 0, BUF_SIZE * sizeof(char));
                fgets(input_message, BUF_SIZE, stdin);

                for (int i = 0; i < CLI_NUM; i++)
                {
                    if (client_fds[i] != 0)
                    {
                        printf("client_fds[%d]=%d\n", i, client_fds[i]);
                        send(client_fds[i], input_message, BUF_SIZE, 0);
                    }
                }

            }

            if (FD_ISSET(ser_souck_fd, &ser_fdset))
            {
                struct sockaddr_in client_address;
                socklen_t address_len;
                int client_sock_fd = accept(ser_souck_fd, (struct sockaddr*)&client_address, &address_len);
                if (client_sock_fd > 0)
                {
                    int flags = -1;
                    //一个客户端到来分配一个fd，CLI_NUM=3，则最多只能有三个客户端，超过4以后跳出for循环，flags重新被赋值为-1
                    for (int i = 0; i < CLI_NUM; i++)
                    {
                        if (client_fds[i] == 0)
                        {
                            flags = i;
                            client_fds[i] = client_sock_fd;
                            break;
                        }
                    }


                    if (flags >= 0)
                    {
                        //printf("new user client[%d] add sucessfully!\n", flags);

                    }

                    else //flags=-1
                    {
                        char full_message[] = "the client is full!can't join!\n";
                        memset(input_message, 0, BUF_SIZE * sizeof(char));
                        strncpy(input_message, full_message, 100);
                        send(client_sock_fd, input_message, BUF_SIZE, 0);

                    }
                }
            }

        }

        //deal with the message

        for (int i = 0; i < CLI_NUM; i++)
        {
            if (client_fds[i] != 0)
            {
                if (FD_ISSET(client_fds[i], &ser_fdset))
                {
                    memset(buff_up_char, 0, BUF_SIZE * sizeof(char));
                    int byte_num = read(client_fds[i], buff_up_char, BUF_SIZE);
                    if (byte_num > 0)
                    {
                        //printf("message form client[%d]:%s\n", i, buff_up_char);

                        char* Gateway_unique_identifier = new char[MAC_address_length];
                        memset(Gateway_unique_identifier, 0, MAC_address_length * sizeof(char));
                        Gateway_unique_identifier[MAC_address_length]='\0';
                        strncpy(Gateway_unique_identifier, buff_up_char+MAC_address_length/2, MAC_address_length);
                        if(strcmp(Gateway_unique_identifier,MAC_address1)==0){
                            buffer_array[0].setData(buff_up_char);
                        }else if(strcmp(Gateway_unique_identifier,MAC_address2)==0){
                            buffer_array[1].setData(buff_up_char);
                        }else if(strcmp(Gateway_unique_identifier,MAC_address3)==0){
                            buffer_array[2].setData(buff_up_char);
                        }else if(strcmp(Gateway_unique_identifier,MAC_address4)==0){
                            buffer_array[3].setData(buff_up_char);
                        }

                        //printf("buffer1: %s\n", buffer1.data);
                        //TODO: 检查socketexample初始时单个网关是否会导致多接收/以及这里的buffer是否，若是则需要换框架
                        //printf("buffer2: %s\n", buffer2.data);

                        for(int loopcount=0; loopcount<=buffer_num-1; loopcount++){
                            buffer_array[loopcount].setIndex();
                            buffer_array[loopcount].uint[BUF_SIZE] = {0};
                            buffer_array[loopcount].setUint();
                        }

                        /* -------------------------------------------------------------------------- */
                        /* --- STAGE : 对中间数据buffer_inter纠错---------------------- */
                        //TODO: false and true带来的多个包同时转发


                        for(int loopcount=0; loopcount<=buffer_num-1; loopcount++){
                            buffer_array[loopcount].setInter(); //接收到的Upstream JSON data structure
#if DEBUG
                            cout<<"buffer"<<loopcount+1<<".inter: "<<buffer_array[loopcount].inter<<endl;
#endif
                            buffer_array[loopcount].setInter_Uint();
                        }

                        /* -------------------------------------------------------------------------- */
                        /* --- STAGE : select的异步处理---------------------- */

                        Rxpk rxpk_array[buffer_num];

                        for(int loopcount=0; loopcount<=buffer_num-1; loopcount++)  rxpk_array[loopcount].setTime(buffer_array[loopcount].uint,buffer_array->buff_index);

                        switch(countED(buffer_array, buffer_num)){
                            case 4:{
                                if(compareTime(rxpk_array, buffer_num)){

                                    printf("buffer_send1: ");
                                    for (int count = 0; count < buffer_array[0].index; count++) {
                                        printf("%02X", buffer_array[0].inter_uint[count]);
                                    }
                                    printf("\n");

                                    printf("buffer_send2: ");
                                    for (int count = 0; count < buffer_array[1].index; count++) {
                                        printf("%02X", buffer_array[1].inter_uint[count]);
                                    }
                                    printf("\n");

                                    printf("buffer_send3: ");
                                    for (int count = 0; count < buffer_array[2].index; count++) {
                                        printf("%02X", buffer_array[2].inter_uint[count]);
                                    }
                                    printf("\n");

                                    printf("buffer_send4: ");
                                    for (int count = 0; count < buffer_array[3].index; count++) {
                                        printf("%02X", buffer_array[3].inter_uint[count]);
                                    }
                                    printf("\n");

                                    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                                        cout<<"buffer"<<loopcount+1<<".inter: "<<buffer_array[loopcount].inter<<endl;
                                        send(sock_up, (void*)buffer_array[loopcount].inter_uint, buffer_array[loopcount].index, 0);
                                    }

                                    printf("\n");

                                }
                                break;
                            }
                        }
                    }
                    else if (byte_num < 0)
                    {
                        printf("rescessed error!");
                    }

                        //某个客户端退出
                    else  //cancel fdset and set fd=0
                    {
                        //printf("clien[%d] exit!\n", i);
                        FD_CLR(client_fds[i], &ser_fdset);
                        client_fds[i] = 0;
                        // printf("clien[%d] exit!\n",i);
                        continue;  //这里如果用break的话一个客户端退出会造成服务器也退出。
                    }
                }
            }
        }


    }
    return 0;

}