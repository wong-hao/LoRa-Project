#pragma once

//send: 假设已经接收到，将其纠错后发送

#include <stdint.h>
#include <stdbool.h>
#include <cstring>
#include <functional>

/* -------------------------------------------------------------------------- */
/* --- STAGE ---------------------- */


int FindFirstSubchar(char* fullchar, char* subchar);

int FindSecondSubchar(char* fullchar, char* subchar);

/* -------------------------------------------------------------------------- */
/* --- STAGE ---------------------- */


void deleteChar(char* array, int location);

/* -------------------------------------------------------------------------- */
/* --- STAGE ---------------------- */

int compareDevAddr(Rxpk* rxpk_array, int array_length);

int compareStat(Rxpk* rxpk_array, int array_length);

int compareCRC(Rxpk* rxpk_array, int array_length);

int compareCRC2(Rxpk* rxpk_array, int array_length);

int compareRSSI(Rxpk* rxpk_array, int array_length);