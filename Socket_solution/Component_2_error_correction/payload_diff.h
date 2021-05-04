#pragma once

#include <ctime>
#include <cstring>
#include <cstdio>
#include <cstdint>

/* -------------------------------------------------------------------------- */
/* --- MACROS --------------------------------------------------------------- */

#define TAKE_N_BITS_FROM(b, p, n) (((b) >> (p)) & ((1 << (n)) - 1))



/* -------------------------------------------------------------------------- */
/* --- STAGE ---------------------- */


void getNe(uint8_t* array1, uint8_t* array2, int size, int& number);