
#include <stdio.h>
#include <string.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <stdio.h>
#include <string.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <stdio.h>
#include <algorithm>
#include <ctime>

#include <stdint.h>    
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <stdio.h>
#include <algorithm>

#include <stdlib.h>
#include <stdint.h>

#define BUF_SIZE 140000



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

char s[256], d[256]; //s «Merged error mask£ªd «Error candidate pattern
void outmystr(int n, char* input, int compare, char* interoutput, char* finaloutput, int length, int& flag, int& test); //https://bbs.csdn.net/topics/399153127

int FindSubchar(char* fullchar, char* subchar);

void getStat(char* char1, char* char2, char* char3);

void getCrc(char* char1, char* char2, char* char3, char* char4);

void getStr(char* char1, char* char2, char* char3, int num);