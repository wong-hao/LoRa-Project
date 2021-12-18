#pragma once

#ifndef HEADER_1_5_H
#define HEADER_1_5_H

//https://download.csdn.net/download/guofu089/4789824?ops_request_misc=%257B%2522request%255Fid%2522%2
//https://blog.csdn.net/songchuwang1868/article/details/89877739/

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


#define MAXEVENTS 64

extern char MAC_address1[];
extern char MAC_address2[];
extern char MAC_address3[];
extern char MAC_address4[];

extern int MAC_address_length;

int make_socket_non_blocking(int sfd);

int create_and_bind();

#endif