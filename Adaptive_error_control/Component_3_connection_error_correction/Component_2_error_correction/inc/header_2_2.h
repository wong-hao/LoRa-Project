#pragma once

#define StageOption 2        //选择纠错方法的顺序

/* -------------------------------------------------------------------------- */
/* --- File log function ---------------------- */

void initFile(char *input);

void openFile();

void logPHYPayload(char *input);

void logCRC(char *input);

void logLine();