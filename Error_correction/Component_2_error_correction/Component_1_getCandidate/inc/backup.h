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

#define BUF_SIZE 4096


void dfs(vector<vector<int>>& res, vector<int>& output, int pos, int len, bool bflag);

vector<vector<int>> qpl(vector<int>& nums);

void output(int n, char* input);

void correct(char* input);