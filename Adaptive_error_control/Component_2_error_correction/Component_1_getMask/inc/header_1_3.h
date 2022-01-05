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


/* -------------------------------------------------------------------------- */
/* --- STAGE ----PC---------- */


void Hex2Bin(char *source, char *dest);
int OZ_bin_xor(const char *s1, char *s2, char *dest);
void Bin2Hex(const char *sSrc, char *sDest);
int OZ_forth_bin_xor(const char *s1, char *s2, char *s3, char *s4, char *dest);

/* --- STAGE ----APC---------- */

int OZ_bin_and(const char *s1, char *s2, char *dest);
void majorityVoting(char *input1, char *input2, char *input3, char *input4, char *output);
void LeastReliableMask(char *input1, char *input2, char *input3, char *input4, char *output, int &Hammming_weight_now);

/* --- STAGE ----soft decoding---------- */
double getPositiveWeight(char input1, char input2, char input3, char input4);
double processData(char input1, char input2, char input3, char input4, float snr1, float snr2, float snr3, float snr4);
void softDecoding(char *input1, char *input2, char *input3, char *input4, char *output, float snr1, float snr2, float snr3, float snr4);

#endif