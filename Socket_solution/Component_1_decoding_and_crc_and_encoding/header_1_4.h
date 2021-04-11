#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>

#define BUF_SIZE 4096


/* -------------------------------------------------------------------------- */
/* --- STAGE ---------------------- */


void lora_crc16(const char data, int* crc);

uint16_t sx1302_lora_payload_crc(const uint8_t* data, uint8_t size);