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


#define BUF_SIZE 1024


/* -------------------------------------------------------------------------- */
/* --- STAGE ----PC---------- */


void Hex2Bin(char *source, char *dest);
int OZ_bin_xor(const char *s1, char *s2, char *dest);
void Bin2Hex(const char *sSrc, char *sDest);

/* --- STAGE ----APC---------- */

int OZ_bin_and(const char *s1, char *s2, char *dest);
void majorityVoting(char *input1, char *input2, char *input3, char *input4, char *output);
void LeastReliableMask(char *input1, char *input2, char *input3, char *input4, char *output);

/* --- STAGE ----MPC---------- */
int ranksixfold(int a, int b, int c, int d, int e, int f);
void mpc(char *a, char *b, char *c, char *d);

#endif