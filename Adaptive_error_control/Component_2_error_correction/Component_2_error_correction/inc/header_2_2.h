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

void logCRC(char *input);

void logPHYPayload(uint8_t *input, int size);

void logFcnt(int input);

void logPDRA(double input);

void logTimestamp(char *input);

void logTime(int input);

void logThroughoutData(double input);

void logThroughout(double input);

void logLine();

#endif