#pragma once

//send: 假设已经接收到，将其纠错后发送

#include <inttypes.h> /* PRIx64, PRIu64... */
#include <stdbool.h>  /* bool type */
#include <stdint.h>   /* C99 types */
#include <stdio.h>    /* printf, fprintf, snprintf, fopen, fputs */

#include <errno.h>    /* error messages */
#include <math.h>     /* modf */
#include <signal.h>   /* sigaction */
#include <stdlib.h>   /* atoi, exit */
#include <string.h>   /* memset */
#include <sys/time.h> /* timeval */
#include <time.h>     /* time, clock_gettime, strftime, gmtime */
#include <unistd.h>   /* getopt, access */

#include <arpa/inet.h>  /* IP address conversion stuff */
#include <netdb.h>      /* gai_strerror */
#include <netinet/in.h> /* INET constants and stuff */
#include <sys/socket.h> /* socket specific definitions */

#include <pthread.h>

static char serv_addr[64] = "106.14.134.224";
static char serv_port_up[8] = "1700"; /* server port for upstream traffic */

static int sock_up; /* socket for upstream traffic */

#define BUF_SIZE 14400


/* -------------------------------------------------------------------------- */
/* --- STAGE ---------------------- */

void Char2Uint(char *array, uint8_t *array_uint);
void Uint2Char(uint8_t *array_uint, char *array, int length);