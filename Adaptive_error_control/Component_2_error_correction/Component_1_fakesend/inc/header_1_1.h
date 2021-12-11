#pragma once

#ifndef HEADER_1_1_H
#define HEADER_1_1_H

//send: 假设已经接收到，将其纠错后发送

#include <cstdint>         /* C99 types */
       /* bool type */
#include <cstdio>          /* printf, fprintf, snprintf, fopen, fputs */
#include <cinttypes>       /* PRIx64, PRIu64... */

#include <cstring>         /* memset */
#include <csignal>         /* sigaction */
#include <ctime>           /* time, clock_gettime, strftime, gmtime */
#include <ctime>       /* timeval */
#include <unistd.h>         /* getopt, access */
#include <cstdlib>         /* atoi, exit */
#include <cerrno>          /* error messages */
#include <cmath>           /* modf */

#include <sys/socket.h>     /* socket specific definitions */
#include <netinet/in.h>     /* INET constants and stuff */
#include <arpa/inet.h>      /* IP address conversion stuff */
#include <netdb.h>          /* gai_strerror */

#include <pthread.h>

#define BUF_SIZE 1024

#define DEBUG 0

/* -------------------------------------------------------------------------- */
/* --- STAGE ----字符转换---------------- */

void Char2Uint(char* array, uint8_t* array_uint);
void Uint2Char(uint8_t* array_uint, char* array, int length);

/* -------------------------------------------------------------------------- */
/* --- STAGE ----socket通信---------------- */

int create_up_socket();

#endif