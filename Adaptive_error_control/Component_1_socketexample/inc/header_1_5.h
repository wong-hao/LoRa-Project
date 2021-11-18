#pragma once

// https://download.csdn.net/download/guofu089/4789824?ops_request_misc=%257B%2522request%255Fid%2522%2
// https://blog.csdn.net/songchuwang1868/article/details/89877739/

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
#define BUF_SIZE 2048

static int make_socket_non_blocking(int sfd);

static int create_and_bind();