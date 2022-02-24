#pragma once

#ifndef HEADER_1_3_H
#define HEADER_1_3_H

//https://blog.csdn.net/weixin_30279751/article/details/95437814

#include <algorithm>
#include <cstdint> /* C99 types */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "header_1_1.h"

/* -------------------------------------------------------------------------- */
/* --- STAGE ----PC---------- */

void Hex2Bin(char *source, char *dest);
int OZ_bin_xor(const char *s1, char *s2, char *dest);
void Bin2Hex(const char *sSrc, char *sDest);

/* --- STAGE ----APC---------- */

int OZ_bin_and(const char *s1, char *s2, char *dest);
void getOutput1(char (*input)[BUF_SIZE], int row, size_t size, char *output, int &Hammming_weight_now);
void getOutput2(char (*input)[BUF_SIZE], int row, size_t size, char *output);
void majorityVoting(char (*input)[BUF_SIZE], int input_size, char *output);
void LeastReliableMask(char (*input)[BUF_SIZE], int input_size, char *output, int &Hammming_weight_now);

/* --- STAGE ----soft decoding---------- */
void processData(char (*input)[BUF_SIZE], size_t size, const float *SNRArray, int row, char *output);
void softDecoding(char (*input)[BUF_SIZE], int input_size, char *output, const float *SNRArray);

#endif