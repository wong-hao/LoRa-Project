#pragma once

#ifndef HEADER_1_1_H
#define HEADER_1_1_H

//send: 假设已经接收到，将其纠错后发送

/* bool type */
#include <cstdint>  /* C99 types */
#include <cstdio>   /* printf, fprintf, snprintf, fopen, fputs */

#include <cerrno>    /* error messages */
#include <cmath>     /* modf */
#include <csignal>   /* sigaction */
#include <cstdlib>   /* atoi, exit */
#include <cstring>   /* memset */
#include <sys/time.h> /* timeval */
#include <ctime>     /* time, clock_gettime, strftime, gmtime */
#include <unistd.h>   /* getopt, access */

#include <arpa/inet.h>  /* IP address conversion stuff */
#include <netdb.h>      /* gai_strerror */
#include <netinet/in.h> /* INET constants and stuff */
#include <sys/socket.h> /* socket specific definitions */

#include <pthread.h>

#define BUF_SIZE 1024

#define DEBUG 0

extern char serv_addr[64];
extern char serv_port_up[8]; /* server port for upstream traffic */
extern int sock_up; /* socket for upstream traffic */

/* -------------------------------------------------------------------------- */
/* --- STAGE ----字符转换---------------- */

void Char2Uint(char *array, uint8_t *array_uint);
void Uint2Char(uint8_t *array_uint, char *array, int length);

/* -------------------------------------------------------------------------- */
/* --- STAGE ----socket通信---------------- */

int create_up_socket();

#endif
