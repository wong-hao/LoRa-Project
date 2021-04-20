//https://www.cnblogs.com/wuyepeng/p/9726771.html

#include"header_1_1.h"
#include"header_1_2.h"
#include"header_1_3.h"
#include"header_1_4.h"
#include"header_1_6.h"

#include"header_2_1.h"
#include"header_2_2.h"

void Char2Uint(char* array, uint8_t* array_uint, int length) {

    for (int count = 0; count < 2 * length; count++) {
        if (count % 2 == 0) {
            char buff_char[BUF_SIZE] = { 0 };
            strncpy(buff_char, array + count, 2); //https://blog.csdn.net/zmhawk/article/details/44600075
            buff_char[strlen(buff_char)] = '\0';
            sscanf(buff_char, "%X", (int*)(&array_uint[count / 2])); //https://bbs.csdn.net/topics/391935459
        }
    }
}

void Uint2Char(uint8_t* array_uint, char* array, int length) {


    for (uint16_t count = 0; count < length; count++) {
        char buff[BUF_SIZE] = { 0 };
        sprintf(buff, "%02X", array_uint[count]);
        strcat(array, buff);

    }

}

int FindFirstSubchar(char* fullchar, char* subchar) {

    char* buffer = strstr(fullchar, subchar); //用于接受返回值

    if (buffer == NULL)
    { //说明没有要找的字符串
        return -1;
    }
    else
    {                                 //说明找到了那个字符串
        return buffer - fullchar + 1; //cde的地址减去abcde的地址+1
    }

}

int FindSecondSubchar(char* fullchar, char* subchar)
{

    char* buffer = strstr(fullchar, subchar); //接收返回值
    int location;

    if (buffer == NULL)
    {
        return -1;
    }
    else
    {
        location = buffer - fullchar + strlen(subchar); //找到第一个subchar的位置
    }

    char fullchar_two[BUF_SIZE];
    memset(fullchar_two, 0, BUF_SIZE * sizeof(char));
    strncpy(fullchar_two, fullchar + location, strlen(fullchar) - location); //fullchar_two是fullchar删除了第一个subchar剩下的字符串
    fullchar_two[strlen(fullchar_two)] = '\0';

    char* buffer_two = strstr(fullchar_two, subchar);
    if (buffer_two == NULL)
    {
        return -1;
    }
    else
    {
        return buffer_two - fullchar_two + 1 + location;
    }
}

void getStat(char* char1, char* char2, char* char3, char* char4) {

    strncpy(char1, char2 + FindFirstSubchar(char2, char3) + 5, FindFirstSubchar(char2, char4) - FindFirstSubchar(char2, char3) - 8); //https://blog.csdn.net/zmhawk/article/details/44600075
}

void getCrc(char* char1, char* char2, char* char3, char* char4) {

    strncpy(char1, char2 + FindFirstSubchar(char2, char3) + 4, FindFirstSubchar(char2, char4) - FindFirstSubchar(char2, char3) - 7); //https://blog.csdn.net/zmhawk/article/details/44600075
}


void getStr(char* char1, char* char2, char* char3, char* char4) {
    strncpy(char1, char2 + FindFirstSubchar(char2, char3) + 6, FindFirstSubchar(char2, char4) - FindFirstSubchar(char2, char3) - 8); //https://blog.csdn.net/zmhawk/article/details/44600075
}

void getRssis(char* char1, char* char2, char* char3, char* char4) {
    strncpy(char1, char2 + FindFirstSubchar(char2, char3) + 6, FindFirstSubchar(char2, char4) - FindFirstSubchar(char2, char3) - 9);
}

void getRssi(char* char1, char* char2, char* char3, char* char4) {
    strncpy(char1, char2 + FindSecondSubchar(char2, char3) + 5, FindFirstSubchar(char2, char4) - FindFirstSubchar(char2, char3) - 44);
}

void getTime(char* char1, char* char2, char* char3, char* char4) {

    strncpy(char1, char2 + FindFirstSubchar(char2, char3) + 4, FindFirstSubchar(char2, char4) - FindFirstSubchar(char2, char3) - 7); //https://blog.csdn.net/zmhawk/article/details/44600075
}


int main() {


    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 建立发射socket ---------------------- */

    /* network socket creation */ //socket套接字网络通信
    struct addrinfo hints;
    struct addrinfo* result; /* store result of getaddrinfo */
    struct addrinfo* q; /* pointer to move into *result data */
    char host_name[64];
    char port_name[64];

    int i; /* loop variable and temporary variable for return value */


/* prepare hints to open network sockets */ //既为upstream也为downstream打基础
    memset(&hints, 0, sizeof hints); //hints
    hints.ai_family = AF_INET; //IPV4 /* WA: Forcing IPv4 as AF_UNSPEC(IPV4 and IPV6) makes connection on localhost to fail */
    hints.ai_socktype = SOCK_DGRAM; //UDP
    //hints.ai_protocol取默认值0，系统会自动推演出应该使用UDP协议

    /* look for server address w/ upstream port */
    i = getaddrinfo(serv_addr, serv_port_up, &hints, &result);
    //serv_addr、serv_port_up由parse_gateway_configuration得出；从hints读取信息存储到result
    //因为IP + port -> socket
    if (i != 0) {
        printf("ERROR: [up] getaddrinfo on address %s (PORT %s) returned %s\n", serv_addr, serv_port_up, gai_strerror(i));
        exit(EXIT_FAILURE);
    }

    /* try to open socket for upstream traffic */
    for (q = result; q != NULL; q = q->ai_next) { //q指向result，q的属性都是上面getaddrinfo得到的；因为一个域名可能不止一个IP地址，所以，需要遍历res中的next，如下，是否还有下一个节点
        sock_up = socket(q->ai_family, q->ai_socktype, q->ai_protocol); //创建套接字sock_up
        if (sock_up == -1) continue; /* try next field */
        else break; /* success, get out of loop */ //得到sock_up后跳出for循环，没有必要循环到结束条件q==NULL
    }
    if (q == NULL) { //一直循环到了结束条件q==NULL都没获得sock_up
        printf("ERROR: [up] failed to open socket to any of server %s addresses (port %s)\n", serv_addr, serv_port_up);
        i = 1;
        for (q = result; q != NULL; q = q->ai_next) {
            getnameinfo(q->ai_addr, q->ai_addrlen, host_name, sizeof host_name, port_name, sizeof port_name, NI_NUMERICHOST);
            //与getaddrinfo两级反转: socket -> IP + port
            printf("INFO: [up] result %i host:%s service:%s\n", i, host_name, port_name);
            ++i;
        }
        exit(EXIT_FAILURE); //异常退出程序
    }

    /* connect so we can send/receive packet with the server only */
    i = connect(sock_up, q->ai_addr, q->ai_addrlen); //连接上行socket；q为for循环break出时的值
    if (i != 0) {
        printf("ERROR: [up] connect returned %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(result); //释放掉好进行下行通信

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
                    	
                        if (resv_message[23] == '6') {
                            strcpy(buffer1, resv_message);
                        }
                        else if (resv_message[23] == '7') {
                            strcpy(buffer2, resv_message);
                        }

                        delete[] resv_message;

                        //printf("buffer1: %s\n", buffer1);

                        //printf("buffer2: %s\n", buffer2);

                        int buff_index1 = strlen(buffer1) / 2;
                        int buff_index2 = strlen(buffer2) / 2;

                        uint8_t  buffer_uint1[BUF_SIZE] = { 0 };
                        uint8_t  buffer_uint2[BUF_SIZE] = { 0 };
                        Char2Uint(buffer1, buffer_uint1, buff_index1);
                        Char2Uint(buffer2, buffer_uint2, buff_index2);


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
                        /* --- STAGE : 发送---------------------- */

                        char report10[BUF_SIZE] = "time";
                        char report11[BUF_SIZE] = "tmms";

                        /* -------------------------------------------------------------------------- */
						/* --- STAGE : select的异步处理---------------------- */
                        char* time1 = new char[BUF_SIZE];
                        memset(time1, 0, BUF_SIZE * sizeof(char));
                        char* time2 = new char[BUF_SIZE];
                        memset(time2, 0, BUF_SIZE * sizeof(char));
                        
                        if(buff_index1 != 0 && buff_index2 != 0){
						getTime(time1, buffer1_inter, report10, report11);
                        getTime(time2, buffer2_inter, report10, report11);

                        	if(strcmp(time1, time2) == 0)
                        	{
                                send(sock_up, (void*)buffer1_inter_uint, buff_index1, 0);
                                send(sock_up, (void*)buffer2_inter_uint, buff_index2, 0);

                                /* -------------------------------------------------------------------------- */
								/* --- STAGE : 以两者发送时重复一个rxinfo为代价换取能够单独发送成功---------------------- */
                        		
                                memset(buffer1, 0, BUF_SIZE * sizeof(char));
                                memset(buffer2, 0, BUF_SIZE * sizeof(char));
                        	}
						}else if (buff_index1 == 0 && buff_index2 != 0) {
                            send(sock_up, (void*)buffer2_inter_uint, buff_index2, 0);
                        }else if (buff_index1 != 0 && buff_index2 == 0) {
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

