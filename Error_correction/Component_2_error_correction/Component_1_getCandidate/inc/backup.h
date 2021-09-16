//https://bbs.csdn.net/topics/600597921

#pragma once

#include <stdio.h>
#include <string.h> //十六进制字符串转二进制字符串
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
#include <iostream>
#include <vector>
#include <iostream>
#include <sstream>
using namespace std;


void dfs(vector<vector<int>>& res, vector<int>& output, int pos, int len, bool bflag);

vector<vector<int>> qpl(vector<int>& nums);

void output(int n, char* input, char* mch, int crc_int, char* fakeresult, char* realresult, int length, int& pass_crc, int& total_number);

void correct(char* input, char* mch, int Hamming_weight_now, int crc_int, char* fakeresult, char* realresult, int length, int& pass_crc, int& total_number);