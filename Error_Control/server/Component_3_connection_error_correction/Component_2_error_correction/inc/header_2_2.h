#pragma once

#ifndef HEADER_2_2_H
#define HEADER_2_2_H

#include <cstdio>

#include "header_1_1.h"

#define StageOption 1//选择纠错方法的顺序（如果仅需要某种算法需要结合使用'break'）

extern struct timespec ProStartTime;

extern FILE *outfile;
extern char fileName[BUF_SIZE];
extern char fileType[BUF_SIZE];

/* -------------------------------------------------------------------------- */
/* --- Overall File log function ---------------------- */

void initFile();

void openFile();

void logCRC(char *input);//deprecated because the pseudo randomness of LMIC

void logData(const char *input);//deprecated because hard to confuse the dataset with different GW number

void logPHYPayload(uint8_t *input, int size);//deprecated because the error locations are too similar

void logFcnt(int input);//deprecated because it is easy to find out the packet loss

void logPDRA(double input);

void logTimestamp(struct timespec endTime);

void logTime(int input);

void logThroughputData(double input);//deprecated because it is easy to find the law of error

void logThroughput(double input);

void logLine();

/* -------------------------------------------------------------------------- */
/* --- Error File log function ---------------------- */

void initErrorFile();

void openErrorFile();

void logHammingWeight(double input);//log in another file because it is easy to find the law of error

void logResult(bool input);

void logErrorLine() ;

#endif