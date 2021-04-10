
//send: 假设已经接收到，将其纠错后发送

#include <stdint.h>         
#include <stdbool.h>        


#define BUF_SIZE 140000



/* -------------------------------------------------------------------------- */
/* --- STAGE ---------------------- */


int FindSubchar(char* fullchar, char* subchar);