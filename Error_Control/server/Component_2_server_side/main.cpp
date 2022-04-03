//https://www.cnblogs.com/wuyepeng/p/9726771.html

#include"header_1_1.h"
#include"header_1_2.h"
#include"header_1_3.h"
#include"header_1_6.h"
#include "parson.h"
#include "header_2_1.h"

extern char serv_addr[];
extern char serv_port_up[];
extern int sock_up;

extern char MAC_address1[];
extern char MAC_address2[];
extern char MAC_address3[];
extern char MAC_address4[];
extern int MAC_address_length;

int main() {


    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 建立发射socket ---------------------- */

    printf("The error control server (port: %d) waits for connections and forward to Network server (address: %s, port: %s)!\n\n", ser_port, serv_addr, serv_port_up);

    int i = create_up_socket();
    if (i==-1) abort();

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 开始处理数据 ---------------------- */

    int ser_souck_fd;
    char input_message[BUF_SIZE] = {0};
    char* resv_message = new char[BUF_SIZE];
	

    char* buffer1 = new char[BUF_SIZE];
    memset(buffer1, 0, BUF_SIZE * sizeof(char));
    char* buffer2 = new char[BUF_SIZE];
    memset(buffer2, 0, BUF_SIZE * sizeof(char));

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

    // 设置套接字选项避免地址使用错误 (https://www.cnblogs.com/argenbarbie/p/4118783.html)
    int on=1;
    if((setsockopt(ser_souck_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)
    {
        perror("setsockopt failed");

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
    printf("wait for client connnect!\n");

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
            //printf("time out!");
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
                        printf("new user client[%d] add sucessfully!\n", flags);

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
                    memset(resv_message, 0, BUF_SIZE * sizeof(char));

                	int byte_num = read(client_fds[i], resv_message, BUF_SIZE);
                    if (byte_num > 0)
                    {
                        //printf("message form client[%d]:%s\n", i, resv_message);

                        char* Gateway_unique_identifier = new char[MAC_address_length];
                        memset(Gateway_unique_identifier, 0, MAC_address_length * sizeof(char));
                        Gateway_unique_identifier[MAC_address_length]='\0';
                        strncpy(Gateway_unique_identifier, resv_message+MAC_address_length/2, MAC_address_length);
                        if(strcmp(Gateway_unique_identifier,MAC_address1)==0){
                            strcpy(buffer1, resv_message);
                        }else if(strcmp(Gateway_unique_identifier,MAC_address2)==0){
                            strcpy(buffer2, resv_message);
                        }

                        delete[] resv_message;

/*测试代码
                        printf("buffer1: %s\n", buffer1);
                        printf("buffer2: %s\n", buffer2);
*/

                        int buff_index1 = strlen(buffer1) / 2;
                        int buff_index2 = strlen(buffer2) / 2;

                        uint8_t  buffer_uint1[BUF_SIZE] = { 0 };
                        uint8_t  buffer_uint2[BUF_SIZE] = { 0 };
                        Char2Uint(buffer1, buffer_uint1);
                        Char2Uint(buffer2, buffer_uint2);


                        /* -------------------------------------------------------------------------- */
                        /* --- STAGE : 对中间数据buffer_inter纠错---------------------- */
                        //TODO: false and true带来的多个包同时转发；根据rssi纠错（必须两个都错以降低时间复杂度）；判断纠错的两个包的crc值是否相同


                        char* buffer1_inter = (char*)(buffer_uint1 + 12);
                        char* buffer2_inter = (char*)(buffer_uint2 + 12);
                        //printf("buffer1_inter: %s\n", buffer1_inter);
                        //printf("\n");
                        //printf("buffer2_inter: %s\n", buffer2_inter);
                        //printf("\n");

                        uint8_t* buffer1_inter_uint = (uint8_t*)(buffer1_inter - 12);
                        uint8_t* buffer2_inter_uint = (uint8_t*)(buffer2_inter - 12);

                        /* -------------------------------------------------------------------------- */
						/* --- STAGE : select的异步处理---------------------- */

                        Rxpk rxpk1{};
                        Rxpk rxpk2{};

                        rxpk1.time = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint1 + 12))), "rxpk"), 0), "time");
                        rxpk2.time = json_object_get_string(json_array_get_object(json_object_get_array(json_value_get_object(json_parse_string_with_comments((const char*)(buffer_uint2 + 12))), "rxpk"), 0), "time");

                        if(buff_index1 != 0 && buff_index2 != 0){

                        	if(strcmp(rxpk1.time, rxpk2.time) == 0)
                        	{
                                send(sock_up, (void*)buffer1_inter_uint, buff_index1, 0);
                                send(sock_up, (void*)buffer2_inter_uint, buff_index2, 0);

                                /* -------------------------------------------------------------------------- */
								/* --- STAGE : 以两者发送时重复一个rxinfo为代价换取能够单独发送成功---------------------- */
                        		
                                memset(buffer1, 0, BUF_SIZE * sizeof(char));
                                memset(buffer2, 0, BUF_SIZE * sizeof(char));
                        	}
						}else if (buff_index1 == 0 && buff_index2 != 0) {
                            printf("Only the %s gateway received message and transferd it\n\n", MAC_address2);
                            send(sock_up, (void*)buffer2_inter_uint, buff_index2, 0);

                        }else if (buff_index1 != 0 && buff_index2 == 0) {
                            printf("Only the %s gateway received message and transferd it\n\n", MAC_address1);
                            send(sock_up, (void*)buffer1_inter_uint, buff_index1, 0);

                        }

                    }
                    else if (byte_num < 0)
                    {
                        printf("rescessed error!");
                    }

                    //某个客户端退出
                    else  //cancel fdset and set fd=0
                    {
                        printf("clien[%d] exit!\n", i);
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

