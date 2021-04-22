//https://download.csdn.net/download/guofu089/4789824?ops_request_misc=%257B%2522request%255Fid%2522%2
//https://blog.csdn.net/songchuwang1868/article/details/89877739/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

#define MAXEVENTS 64
#define BUF_SIZE 14400

static int
make_socket_non_blocking(int sfd);

static int
create_and_bind();