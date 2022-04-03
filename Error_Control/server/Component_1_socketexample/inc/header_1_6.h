#pragma once

#ifndef HEADER_1_6_H
#define HEADER_1_6_H

// https://www.cnblogs.com/wuyepeng/p/9726771.html
// https://blog.csdn.net/songchuwang1868/article/details/89877739/

#include <errno.h>
#include <netinet/in.h> //for souockaddr_in
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>

// for select
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <string.h>
#include <strings.h> //for bzero

#define BUF_SIZE 4096
#define backlog 7
#define ser_port 1680
#define CLI_NUM 100

#define MAC_address1 "0016C001FF10D3F6"
#define MAC_address2 "0016C001FF10D3F7"
#define MAC_address3 "0016C001FF10D42D"
#define MAC_address4 "0016C001FF10D446"

extern int MAC_address_length;

extern int client_fds[CLI_NUM];

#endif