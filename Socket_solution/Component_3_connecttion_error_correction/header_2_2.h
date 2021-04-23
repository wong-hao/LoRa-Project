//send: 假设已经接收到，将其纠错后发送

#include <stdint.h>         
#include <stdbool.h>        




/* -------------------------------------------------------------------------- */
/* --- STAGE ---------------------- */


int FindFirstSubchar(char* fullchar, char* subchar);

int FindSecondSubchar(char* fullchar, char* subchar);

void getStat(char* char1, char* char2, char* char3, char* char4);

void getCrc(char* char1, char* char2, char* char3, char* char4);

void getStr(char* char1, char* char2, char* char3, char* char4);

void getRssis(char* char1, char* char2, char* char3, char* char4);

void getRssi(char* char1, char* char2, char* char3, char* char4);

void getTime(char* char1, char* char2, char* char3, char* char4);