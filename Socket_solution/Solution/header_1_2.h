#pragma once

//https://bbs.csdn.net/topics/399153127
#include <stdio.h>
#include <string.h> //ʮ�������ַ���ת�������ַ���
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> /* C99 types */
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <stdio.h>
#include <algorithm>

#define BUF_SIZE 14400

char s[BUF_SIZE], d[BUF_SIZE]; //s��Merged error mask��d��Error candidate pattern

/* -------------------------------------------------------------------------- */
/* --- STAGE ---------------------- */


void outmystr(int n, char* input, char* compare, char* interoutput, char* finaloutput);