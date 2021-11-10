#pragma once

#define Hamming_weight_max 30//预设的最多纠错比特位数量
#define StageOption 1        //选择纠错方法的顺序

/* -------------------------------------------------------------------------- */
/* --- File log function ---------------------- */

void initFile();

void openFile();

void logPHYPayload(char *input);

void logCRC(int input);

void logLine();