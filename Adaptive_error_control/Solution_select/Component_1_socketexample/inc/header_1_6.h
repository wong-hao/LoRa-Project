#pragma once

//https://www.cnblogs.com/wuyepeng/p/9726771.html
//https://blog.csdn.net/songchuwang1868/article/details/89877739/

#include <stdio.h>
#include <netinet/in.h>   //for souockaddr_in
#include <sys/types.h>     
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>

#include <arpa/inet.h>

//for select
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>

#include <strings.h>   //for bzero
#include <string.h>

#define backlog 7
#define ser_port 1680
#define CLI_NUM 3


int client_fds[CLI_NUM];

char MAC_address1[] = "0016C001FF10D3F6";
char MAC_address2[] = "0016C001FF10D3F7";
char MAC_address3[] = "0016C001FF10D42D";
char MAC_address4[] = "0016C001FF10D446";

int MAC_address_length = strlen(MAC_address1);
