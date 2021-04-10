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


/* -------------------------------------------------------------------------- */
/* --- STAGE ---------------------- */


void Hex2Bin(char* source, char* dest, int len);
int OZ_bin_xor(const char* s1, char* s2, char* dest);
void Bin2Hex(const char* sSrc, char* sDest, int nSrcLen);

