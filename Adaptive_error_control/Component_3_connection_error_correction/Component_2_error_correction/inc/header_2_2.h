#pragma once

#ifndef HEADER_2_2_H
#define HEADER_2_2_H

#include <cstdio>

#include "header_1_1.h"

#define StageOption 2        //选择纠错方法的顺序

extern FILE *outfile;
extern char fileName[BUF_SIZE];
extern char fileType[BUF_SIZE];

/* -------------------------------------------------------------------------- */
/* --- File log function ---------------------- */

void initFile(char *input);

void openFile();

void logPHYPayload(char *input);

void logCRC(char *input);

void logLine();

#endif