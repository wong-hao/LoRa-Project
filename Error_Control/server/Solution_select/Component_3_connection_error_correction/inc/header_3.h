#pragma once

#ifndef HEADER_3_H
#define HEADER_3_H

//send: 假设已经接收到，将其纠错后发送

#include <cstring>
#include <functional>
#include <cstdint>
#include "timelib.h"

#define FakeOption 1
#define MAXRUNTIME 1800.0 //程序最大运行时间/统计时间
#define GW 4

#define MINPDR 0.0
#define MAXPDR 0.1

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

int compareSize(Buffer *buffer_array, int array_length);

int compareRSSI(Rxpk *rxpk_array, int array_length);//deprecated because of SNR

int compareSNR(Rxpk *rxpk_array, int array_length);

/* -------------------------------------------------------------------------- */
/* --- Statistical Calculation ---------------------- */

void getPER(double compound1, double compound2);//Calculate per tx interval

void getTotalTime(struct timespec endTime, struct timespec startTime);

void getThroughput(double data, struct timespec endTime, struct timespec startTime);//Calculate per tx interval

void checkRuntime(); //Check total runtime limitation

/* -------------------------------------------------------------------------- */
/* --- Fake Calculation ---------------------- */

void controlRange(double* compound1, double* compound2); //Control the range of PDR

#endif