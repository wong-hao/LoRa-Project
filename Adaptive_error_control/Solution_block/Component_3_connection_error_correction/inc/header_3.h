#pragma once

//send: 假设已经接收到，将其纠错后发送

#include <stdint.h>
#include <stdbool.h>
#include <cstring>


/* -------------------------------------------------------------------------- */
/* --- STAGE ---------------------- */


int FindFirstSubchar(char* fullchar, char* subchar);

int FindSecondSubchar(char* fullchar, char* subchar);

/* -------------------------------------------------------------------------- */
/* --- STAGE ---------------------- */


void deleteChar(char* array, int location);