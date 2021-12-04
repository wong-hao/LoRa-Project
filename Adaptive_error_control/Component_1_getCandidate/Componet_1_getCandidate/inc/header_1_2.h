//https://blog.csdn.net/zhao2018/article/details/82803070?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522163133865416780264014767%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=163133865416780264014767&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~rank_v29_ecpm-1-82803070.pc_search_result_cache&utm_term=C%2B%2B%E7%A9%B7%E4%B8%BEn%E4%BD%8D%E4%BA%8C%E8%BF%9B%E5%88%B6%E6%95%B0&spm=1018.2226.3001.4187
//https://blog.csdn.net/sayniceoh/article/details/53105466?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522163133865416780264014767%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=163133865416780264014767&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~rank_v29_ecpm-3-53105466.pc_search_result_cache&utm_term=C%2B%2B%E7%A9%B7%E4%B8%BEn%E4%BD%8D%E4%BA%8C%E8%BF%9B%E5%88%B6%E6%95%B0&spm=1018.2226.3001.4187
//https://www.codeleading.com/article/3956123395/
//https://blog.csdn.net/acsuperman/article/details/79930557
//https://www.geek-share.com/detail/2720457588.html

//https://blog.csdn.net/nerdx/article/details/12561043

#pragma once

#include <algorithm>
#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>
#include <stdint.h> /* C99 types */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/time.h>
#include <vector>

#include "oslmic_types.h"
#include "pgmspace.h"

#define BUF_SIZE 1024

#define Concurrent 1//最多允许通过crc校验的次数
#define Hamming_weight_max 30//预设的最多纠错比特位数量
#define MAXLATENCY 30.0
#define MICOption 1//是否进行MIC校验

static const PROGMEM u1_t NWKSKEY[16] = {0x43, 0x57, 0x9e, 0xa9, 0x9c, 0xf9, 0x25, 0x62, 0x04, 0xd4, 0x77, 0x8f, 0x63, 0xa6, 0x1c, 0x0c};
static const u1_t PROGMEM APPSKEY[16] = {0xe9, 0xb5, 0x3b, 0x90, 0x85, 0x77, 0xe0, 0xcf, 0x4a, 0x7d, 0xbe, 0x49, 0x0d, 0x40, 0xb0, 0x45};
static const u4_t DEVADDR = 0x00deea15;// <-- Change this address for every node!


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