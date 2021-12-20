//https://blog.csdn.net/zhao2018/article/details/82803070?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522163133865416780264014767%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=163133865416780264014767&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~rank_v29_ecpm-1-82803070.pc_search_result_cache&utm_term=C%2B%2B%E7%A9%B7%E4%B8%BEn%E4%BD%8D%E4%BA%8C%E8%BF%9B%E5%88%B6%E6%95%B0&spm=1018.2226.3001.4187
//https://blog.csdn.net/sayniceoh/article/details/53105466?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522163133865416780264014767%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=163133865416780264014767&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~rank_v29_ecpm-3-53105466.pc_search_result_cache&utm_term=C%2B%2B%E7%A9%B7%E4%B8%BEn%E4%BD%8D%E4%BA%8C%E8%BF%9B%E5%88%B6%E6%95%B0&spm=1018.2226.3001.4187
//https://www.codeleading.com/article/3956123395/
//https://blog.csdn.net/acsuperman/article/details/79930557
//https://www.geek-share.com/detail/2720457588.html

//https://blog.csdn.net/nerdx/article/details/12561043

#pragma once

#ifndef HEADER_1_2_H
#define HEADER_1_2_H

#include <algorithm>
#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>
#include <cstdint> /* C99 types */
#include <cstdio>
#include <cstdlib>
#include <string>
#include <ctime>
#include <vector>

#include "oslmic_types.h"
#include "pgmspace.h"

#define BUF_SIZE 1024

#define Concurrent 1         //最多允许通过crc校验的次数
#define Hamming_weight_max 4 * 8//预设的最多纠错比特位数量，防止内存占用过大程序被kill掉，与Latency无关
#define MAXLATENCY 5.0

extern char num[BUF_SIZE];
extern char num2[BUF_SIZE];
extern int global_n;

/* -------------------------------------------------------------------------- */
/* --- Fundamental function ---------------------- */

int validateMIC(uint8_t* payload, int fcnt, int length);

void validateCRC(int crc_int, char *fakeresult, char *realresult, int length, int &pass_crc);

/* -------------------------------------------------------------------------- */
/* --- Correct ---------------------- */

void insertzero(char *input, int location);

void Search(char *input, int m, struct timespec startTime);

void correct(char *input, int Hamming_weight_now, struct timespec startTime);

/* -------------------------------------------------------------------------- */
/* --- Incremental correct ---------------------- */

//https://bbs.csdn.net/topics/600597921

using namespace std;

void dfs(vector<vector<int>> &res, vector<int> &output, int pos, int len, bool bflag);

vector<vector<int>> qpl(vector<int> &nums);

void output(int n, char *input, struct timespec startTime);

void incremental_correct(char *input, int Hamming_weight_now, struct timespec startTime);

#endif