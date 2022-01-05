#pragma once

#ifndef HEADER_2_2_H
#define HEADER_2_2_H

#include <cstdio>

#include "header_1_1.h"

#define StageOption 1//选择纠错方法的顺序

extern struct timespec ProStartTime;

extern FILE *outfile;
extern char fileName[BUF_SIZE];
extern char fileType[BUF_SIZE];

/* -------------------------------------------------------------------------- */
/* --- File log function ---------------------- */

void initFile();

void openFile();

void logCRC(char *input);//deprecated because the pseudo randomness of LMIC

void logData(const char *input);

void logPHYPayload(uint8_t *input, int size);//deprecated because the error locations are too similar

void logFcnt(int input);//deprecated because it is easy to find out the packet loss

void logPDRA(double input);

void logTimestamp(char *input);

void logTime(int input);

void logThroughoutData(double input);//deprecated because it is easy to find the law of error

void logThroughout(double input);

void logLine();

#endif