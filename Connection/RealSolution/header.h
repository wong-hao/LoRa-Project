
//send: 假设已经接收到，将其纠错后发送

#include <stdint.h>         
#include <stdbool.h>        
#include <stdio.h>          
#include <inttypes.h>       

#include <string.h>         
#include <signal.h>         
#include <time.h>           
#include <sys/time.h>       
#include <unistd.h>        
#include <stdlib.h>         
#include <errno.h>         
#include <math.h>          

#include <sys/socket.h>     
#include <netinet/in.h>     
#include <arpa/inet.h>      
#include <netdb.h>          

#include <pthread.h>


#define BUF_SIZE 140000

static char serv_addr[64] = "47.110.36.225";
static char serv_port_up[8] = "1700"; /* server port for upstream traffic */

static int sock_up; /* socket for upstream traffic */


void Uint2Char(uint8_t* array_uint, char* array, int length);

void Char2Uint(char* array, uint8_t* array_uint, int length);

int FindSubchar(char* fullchar, char* subchar);