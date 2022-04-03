//https://github.com/Lora-net/sx1302_hal/blob/4b42025d1751e04632c0b04160e0d29dbbb222a5/tools/payload_tools/payload_diff.c#L71

#pragma once

#include <ctime>
#include <cstring>
#include <cstdio>
#include <cstdint>

/* -------------------------------------------------------------------------- */
/* --- MACROS --------------------------------------------------------------- */

#define TAKE_N_BITS_FROM(b, p, n) (((b) >> (p)) & ((1 << (n)) - 1))



/* -------------------------------------------------------------------------- */
/* --- XOR ---------------------- */

void OZ_hex_xor(uint8_t* array1, uint8_t* array2, uint8_t* array3, int size);

void getNe(uint8_t* array1, uint8_t* array2, int size, int& number);

/* -------------------------------------------------------------------------- */
/* --- Fourth XOR ---------------------- */

void OZ_forth_hex_xor(uint8_t* array1, uint8_t* array2, uint8_t* array3, uint8_t* array4, uint8_t* array5, int size);

void getFourthNe(uint8_t* array1, uint8_t* array2, uint8_t* array3, uint8_t* array4, int size, int& number);