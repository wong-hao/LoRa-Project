#include <stdio.h>
#include <string.h> //十六进制字符串转二进制字符串
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> /* C99 types */

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
#include<ctime>

#include <stdint.h>        /* C99 types */
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <stdio.h>
#include <algorithm>

#include <stdlib.h>
#include <stdint.h>


/*------------------------------------------------------------*/

void lora_crc16(const char data, int* crc);

uint16_t sx1302_lora_payload_crc(const uint8_t* data, uint8_t size);

/*------------------------------------------------------------*/

void Hex2Bin(char* source, char* dest, int len);

int OZ_bin_xor(const char* s1, char* s2, char* dest);

void Bin2Hex(const char* sSrc, char* sDest, int nSrcLen);

void getNe(char* array, int& number);

void Uint2Char(uint8_t* array_uint, char* array, int length);

void Char2Uint(char* array, uint8_t* array_uint, int length);

char s[256], d[256]; //s是Merged error mask；d是Error candidate pattern
void outmystr(int n, char* input, int compare, char* interoutput, char* finaloutput, int length, int& flag, int& test); //https://bbs.csdn.net/topics/399153127