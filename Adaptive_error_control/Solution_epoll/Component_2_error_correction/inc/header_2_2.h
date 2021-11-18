#pragma once

#define StageOption 1        //选择纠错方法的顺序

/* -------------------------------------------------------------------------- */
/* --- File log function ---------------------- */

void initFile(char *input);

void openFile();

void logPHYPayload(char *input);

void logCRC(int input);

void logLine();