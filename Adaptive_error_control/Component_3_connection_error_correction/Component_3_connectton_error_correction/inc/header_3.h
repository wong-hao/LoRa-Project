#pragma once

//send: 假设已经接收到，将其纠错后发送

#include <cstring>
#include <functional>
#include <stdbool.h>
#include <stdint.h>

#define FakeOption 0
#define DeprecatedOption 1

/* -------------------------------------------------------------------------- */
/* --- STAGE ---------------------- */


int FindFirstSubchar(char *fullchar, char *subchar);

int FindSecondSubchar(char *fullchar, char *subchar);

/* -------------------------------------------------------------------------- */
/* --- STAGE ---------------------- */


void deleteChar(char *array, int location);

/* -------------------------------------------------------------------------- */
/* --- STAGE ---------------------- */

int compareDevAddr(Rxpk *rxpk_array, int array_length);

int compareStat(Rxpk *rxpk_array, int array_length);

int compareCRC(Rxpk *rxpk_array, int array_length);//判断是否所有crc值相同

int compareCRC2(Rxpk *rxpk_array, int array_length);//判断是否所有crc值相同

int compareCRC3(Rxpk *rxpk_array);//判断是否有多次出现的crc值

int compareRSSI(Rxpk *rxpk_array, int array_length);//deprecated because of SNR

int compareSNR(Rxpk *rxpk_array, int array_length);


/* -------------------------------------------------------------------------- */
/* --- STAGE ---------------------- */

void replaceData1(char *buff_up_char);//对于单个数据包来说，最后一个3A22到227D5D7D之间的数值对应Json里面"data"部分 / LoRaWAN Frame里的PHY payload (已加密，为了得到FRM payload需要在线解码)

void replaceData2(char *buff_up_char);

void replaceData3(char *buff_up_char);

void replaceData4(char *buff_up_char);

void replaceStat(char *buff_up_char);//最后一个出现的223A312C22插入2D变成223A2D312C22即使stat从1变为-1