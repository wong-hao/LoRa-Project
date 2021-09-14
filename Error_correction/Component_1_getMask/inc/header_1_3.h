#pragma once

//https://blog.csdn.net/weixin_30279751/article/details/95437814

#include <stdio.h>
#include <string.h> //十六进制字符串转二进制字符串
#include <stdlib.h>
#include <string.h>
#include <stdint.h> /* C99 types */
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>


#define BUF_SIZE 4096


/* -------------------------------------------------------------------------- */
/* --- STAGE ----PC---------- */


void Hex2Bin(char* source, char* dest);
int OZ_bin_xor(const char* s1, char* s2, char* dest);
void Bin2Hex(const char* sSrc, char* sDest);

/* --- STAGE ----APC---------- */

int OZ_bin_and(const char* s1, char* s2, char* dest);
void majorityVoting(char* input1, char* input2, char* input3, char* output);
void LeastReliableMask(char* input1, char* input2, char* input3, char* output);

/* --- STAGE ----APC---------- */
int ranktriple(int a, int b, int c);
void mpc(char* a, char* b, char* c);