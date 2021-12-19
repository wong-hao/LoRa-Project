#pragma once

#ifndef HEADER_3_H
#define HEADER_3_H

//send: 假设已经接收到，将其纠错后发送

#include <cstring>
#include <functional>
#include <stdbool.h>
#include <stdint.h>

#define DeprecatedOption 1

extern double TotalPacket;
extern double ErrorPacket;
extern double PEREnv;
extern double PDREnv;

extern double TotalSNR;
extern double SNREnv;

/* -------------------------------------------------------------------------- */
/* --- STAGE ---------------------- */


int FindFirstSubchar(char *fullchar, const char *subchar);

int FindSecondSubchar(char *fullchar, const char *subchar);

/* -------------------------------------------------------------------------- */
/* --- STAGE ---------------------- */


void deleteChar(char *array, int location);

/* -------------------------------------------------------------------------- */
/* --- STAGE ---------------------- */

int compareDevAddr(Rxpk *rxpk_array, int array_length);

int compareStat(Rxpk *rxpk_array, int array_length);

int compareCRC(Rxpk *rxpk_array, int array_length);//判断是否所有crc值相同

int compareCRC2(Rxpk *rxpk_array, int array_length);//判断是否所有crc值相同

int compareCRC3(Rxpk *rxpk_array);//判断是否有多次出现的crc值

int compareRSSI(Rxpk *rxpk_array, int array_length);//deprecated because of SNR

int compareSNR(Rxpk *rxpk_array, int array_length);

#endif