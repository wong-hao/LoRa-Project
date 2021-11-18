#pragma once

//send: 假设已经接收到，将其纠错后发送

#include <stdint.h>         /* C99 types */
#include <stdbool.h>        /* bool type */
#include <stdio.h>          /* printf, fprintf, snprintf, fopen, fputs */
#include <inttypes.h>       /* PRIx64, PRIu64... */

#include <string.h>         /* memset */
#include <signal.h>         /* sigaction */
#include <time.h>           /* time, clock_gettime, strftime, gmtime */
#include <sys/time.h>       /* timeval */
#include <unistd.h>         /* getopt, access */
#include <stdlib.h>         /* atoi, exit */
#include <errno.h>          /* error messages */
#include <math.h>           /* modf */

#include <sys/socket.h>     /* socket specific definitions */
#include <netinet/in.h>     /* INET constants and stuff */
#include <arpa/inet.h>      /* IP address conversion stuff */
#include <netdb.h>          /* gai_strerror */

#include <pthread.h>

#define BUF_SIZE 2048


/* -------------------------------------------------------------------------- */
/* --- STAGE ----字符转换---------------- */

void Char2Uint(char *array, uint8_t *array_uint);

void Uint2Char(uint8_t *array_uint, char *array, int length);

/* -------------------------------------------------------------------------- */
/* --- STAGE ----socket通信---------------- */

int create_up_socket();