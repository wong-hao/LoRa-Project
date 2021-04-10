#include"header_1_4.h"

/* -------------------------------------------------------------------------- */
/* --- STAGE 1: Decoding ---------------------- */

#ifndef _BASE64_H
#define _BASE64_H

/* -------------------------------------------------------------------------- */
/* --- DEPENDANCIES --------------------------------------------------------- */

#include <stdint.h>        /* C99 types */
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <stdio.h>
#include <algorithm>
/* -------------------------------------------------------------------------- */
/* --- PUBLIC FUNCTIONS PROTOTYPES ------------------------------------------ */

/**
@brief Encode binary data in Base64 string (no padding)
@param in pointer to a table of binary data
@param size number of bytes to be encoded to base64
@param out pointer to a string where the function will output encoded data
@param max_len max length of the out string (including null char)
@return >=0 length of the resulting string (w/o null char), -1 for error
*/
int bin_to_b64_nopad(const uint8_t* in, int size, char* out, int max_len);

/**
@brief Decode Base64 string to binary data (no padding)
@param in string containing only base64 valid characters
@param size number of characters to be decoded from base64 (w/o null char)
@param out pointer to a data buffer where the function will output decoded data
@param out_max_len usable size of the output data buffer
@return >=0 number of bytes written to the data buffer, -1 for error
*/
int b64_to_bin_nopad(const char* in, int size, uint8_t* out, int max_len);

/* === derivative functions === */

/**
@brief Encode binary data in Base64 string (with added padding)
*/
int bin_to_b64(const uint8_t* in, int size, char* out, int max_len);

/**
@brief Decode Base64 string to binary data (remove padding if necessary)
*/
int b64_to_bin(const char* in, int size, uint8_t* out, int max_len);

#endif

/* --- EOF ------------------------------------------------------------------ */


/* -------------------------------------------------------------------------- */
/* --- DEPENDANCIES --------------------------------------------------------- */


#include <stdlib.h>
#include <stdint.h>


/* -------------------------------------------------------------------------- */
/* --- PRIVATE MACROS ------------------------------------------------------- */

#define ARRAY_SIZE(a)       (sizeof(a) / sizeof((a)[0]))
#define CRIT(a)             fprintf(stderr, "\nCRITICAL file:%s line:%u msg:%s\n", __FILE__, __LINE__,a);exit(EXIT_FAILURE)

//#define DEBUG(args...)    fprintf(stderr,"debug: " args) /* diagnostic message that is destined to the user */
/* -------------------------------------------------------------------------- */
/* --- PRIVATE CONSTANTS ---------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PRIVATE MODULE-WIDE VARIABLES ---------------------------------------- */

static char code_62 = '+';    /* RFC 1421 standard character for code 62 */
static char code_63 = '/';    /* RFC 1421 standard character for code 63 */
static char code_pad = '=';    /* RFC 1421 padding character if padding */

/* -------------------------------------------------------------------------- */
/* --- PRIVATE FUNCTIONS DECLARATION ---------------------------------------- */

/**
@brief Convert a code in the range 0-63 to an ASCII character
*/
char code_to_char(uint8_t x);

/**
@brief Convert an ASCII character to a code in the range 0-63
*/
uint8_t char_to_code(char x);

/* -------------------------------------------------------------------------- */
/* --- PRIVATE FUNCTIONS DEFINITION ----------------------------------------- */

char code_to_char(uint8_t x) {
    if (x <= 25) {
        return 'A' + x;
    }
    else if ((x >= 26) && (x <= 51)) {
        return 'a' + (x - 26);
    }
    else if ((x >= 52) && (x <= 61)) {
        return '0' + (x - 52);
    }
    else if (x == 62) {
        return code_62;
    }
    else if (x == 63) {
        return code_63;
    }
    else {
        //DEBUG("ERROR: %i IS OUT OF RANGE 0-63 FOR BASE64 ENCODING\n", x);
        //exit(EXIT_FAILURE);
    } //TODO: improve error management
}

uint8_t char_to_code(char x) {
    if ((x >= 'A') && (x <= 'Z')) {
        return (uint8_t)x - (uint8_t)'A';
    }
    else if ((x >= 'a') && (x <= 'z')) {
        return (uint8_t)x - (uint8_t)'a' + 26;
    }
    else if ((x >= '0') && (x <= '9')) {
        return (uint8_t)x - (uint8_t)'0' + 52;
    }
    else if (x == code_62) {
        return 62;
    }
    else if (x == code_63) {
        return 63;
    }
    else {
        //DEBUG("ERROR: %c (0x%x) IS INVALID CHARACTER FOR BASE64 DECODING\n", x, x);
        //exit(EXIT_FAILURE);
    } //TODO: improve error management
}

/* -------------------------------------------------------------------------- */
/* --- PUBLIC FUNCTIONS DEFINITION ------------------------------------------ */

int bin_to_b64_nopad(const uint8_t* in, int size, char* out, int max_len) {
    int i;
    int result_len; /* size of the result */
    int full_blocks; /* number of 3 unsigned chars / 4 characters blocks */
    int last_bytes; /* number of unsigned chars <3 in the last block */
    int last_chars; /* number of characters <4 in the last block */
    uint32_t b;

    /* check input values */
    if ((out == NULL) || (in == NULL)) {
        //DEBUG("ERROR: NULL POINTER AS OUTPUT IN BIN_TO_B64\n");
        return -1;
    }
    if (size == 0) {
        *out = 0; /* null string */
        return 0;
    }

    /* calculate the number of base64 'blocks' */
    full_blocks = size / 3;
    last_bytes = size % 3;
    switch (last_bytes) {
    case 0: /* no byte left to encode */
        last_chars = 0;
        break;
    case 1: /* 1 byte left to encode -> +2 chars */
        last_chars = 2;
        break;
    case 2: /* 2 bytes left to encode -> +3 chars */
        last_chars = 3;
        break;
    default:
        CRIT("switch default that should not be possible");
    }

    /* check if output buffer is big enough */
    result_len = (4 * full_blocks) + last_chars;
    if (max_len < (result_len + 1)) { /* 1 char added for string terminator */
        //DEBUG("ERROR: OUTPUT BUFFER TOO SMALL IN BIN_TO_B64\n");
        return -1;
    }

    /* process all the full blocks */
    for (i = 0; i < full_blocks; ++i) {
        b = (0xFF & in[3 * i]) << 16;
        b |= (0xFF & in[3 * i + 1]) << 8;
        b |= 0xFF & in[3 * i + 2];
        out[4 * i + 0] = code_to_char((b >> 18) & 0x3F);
        out[4 * i + 1] = code_to_char((b >> 12) & 0x3F);
        out[4 * i + 2] = code_to_char((b >> 6) & 0x3F);
        out[4 * i + 3] = code_to_char(b & 0x3F);
    }

    /* process the last 'partial' block and terminate string */
    i = full_blocks;
    if (last_chars == 0) {
        out[4 * i] = 0; /* null character to terminate string */
    }
    else if (last_chars == 2) {
        b = (0xFF & in[3 * i]) << 16;
        out[4 * i + 0] = code_to_char((b >> 18) & 0x3F);
        out[4 * i + 1] = code_to_char((b >> 12) & 0x3F);
        out[4 * i + 2] = 0; /* null character to terminate string */
    }
    else if (last_chars == 3) {
        b = (0xFF & in[3 * i]) << 16;
        b |= (0xFF & in[3 * i + 1]) << 8;
        out[4 * i + 0] = code_to_char((b >> 18) & 0x3F);
        out[4 * i + 1] = code_to_char((b >> 12) & 0x3F);
        out[4 * i + 2] = code_to_char((b >> 6) & 0x3F);
        out[4 * i + 3] = 0; /* null character to terminate string */
    }

    return result_len;
}

int b64_to_bin_nopad(const char* in, int size, uint8_t* out, int max_len) {
    int i;
    int result_len; /* size of the result */
    int full_blocks; /* number of 3 unsigned chars / 4 characters blocks */
    int last_chars; /* number of characters <4 in the last block */
    int last_bytes; /* number of unsigned chars <3 in the last block */
    uint32_t b;
    ;

    /* check input values */
    if ((out == NULL) || (in == NULL)) {
        //DEBUG("ERROR: NULL POINTER AS OUTPUT OR INPUT IN B64_TO_BIN\n");
        return -1;
    }
    if (size == 0) {
        return 0;
    }

    /* calculate the number of base64 'blocks' */
    full_blocks = size / 4;
    last_chars = size % 4;
    switch (last_chars) {
    case 0: /* no char left to decode */
        last_bytes = 0;
        break;
    case 1: /* only 1 char left is an error */
        //DEBUG("ERROR: ONLY ONE CHAR LEFT IN B64_TO_BIN\n");
        return -1;
    case 2: /* 2 chars left to decode -> +1 byte */
        last_bytes = 1;
        break;
    case 3: /* 3 chars left to decode -> +2 bytes */
        last_bytes = 2;
        break;
    default:
        CRIT("switch default that should not be possible");
    }

    /* check if output buffer is big enough */
    result_len = (3 * full_blocks) + last_bytes;
    if (max_len < result_len) {
        //DEBUG("ERROR: OUTPUT BUFFER TOO SMALL IN B64_TO_BIN\n");
        return -1;
    }

    /* process all the full blocks */
    for (i = 0; i < full_blocks; ++i) {
        b = (0x3F & char_to_code(in[4 * i])) << 18;
        b |= (0x3F & char_to_code(in[4 * i + 1])) << 12;
        b |= (0x3F & char_to_code(in[4 * i + 2])) << 6;
        b |= 0x3F & char_to_code(in[4 * i + 3]);
        out[3 * i + 0] = (b >> 16) & 0xFF;
        out[3 * i + 1] = (b >> 8) & 0xFF;
        out[3 * i + 2] = b & 0xFF;
    }

    /* process the last 'partial' block */
    i = full_blocks;
    if (last_bytes == 1) {
        b = (0x3F & char_to_code(in[4 * i])) << 18;
        b |= (0x3F & char_to_code(in[4 * i + 1])) << 12;
        out[3 * i + 0] = (b >> 16) & 0xFF;
        if (((b >> 12) & 0x0F) != 0) {
            //DEBUG("WARNING: last character contains unusable bits\n");
        }
    }
    else if (last_bytes == 2) {
        b = (0x3F & char_to_code(in[4 * i])) << 18;
        b |= (0x3F & char_to_code(in[4 * i + 1])) << 12;
        b |= (0x3F & char_to_code(in[4 * i + 2])) << 6;
        out[3 * i + 0] = (b >> 16) & 0xFF;
        out[3 * i + 1] = (b >> 8) & 0xFF;
        if (((b >> 6) & 0x03) != 0) {
            //DEBUG("WARNING: last character contains unusable bits\n");
        }
    }

    return result_len;
}

int bin_to_b64(const uint8_t* in, int size, char* out, int max_len) {
    int ret;

    ret = bin_to_b64_nopad(in, size, out, max_len);

    if (ret == -1) {
        return -1;
    }
    switch (ret % 4) {
    case 0: /* nothing to do */
        return ret;
    case 1:
        //DEBUG("ERROR: INVALID UNPADDED BASE64 STRING\n");
        return -1;
    case 2: /* 2 chars in last block, must add 2 padding char */
        if (max_len >= (ret + 2 + 1)) {
            out[ret] = code_pad;
            out[ret + 1] = code_pad;
            out[ret + 2] = 0;
            return ret + 2;
        }
        else {
            //DEBUG("ERROR: not enough room to add padding in bin_to_b64\n");
            return -1;
        }
    case 3: /* 3 chars in last block, must add 1 padding char */
        if (max_len >= (ret + 1 + 1)) {
            out[ret] = code_pad;
            out[ret + 1] = 0;
            return ret + 1;
        }
        else {
            //DEBUG("ERROR: not enough room to add padding in bin_to_b64\n");
            return -1;
        }
    default:
        CRIT("switch default that should not be possible");
    }
}

int b64_to_bin(const char* in, int size, uint8_t* out, int max_len) {
    if (in == NULL) {
        // DEBUG("ERROR: NULL POINTER AS OUTPUT OR INPUT IN B64_TO_BIN\n");
        return -1;
    }
    if ((size % 4 == 0) && (size >= 4)) { /* potentially padded Base64 */
        if (in[size - 2] == code_pad) { /* 2 padding char to ignore */
            return b64_to_bin_nopad(in, size - 2, out, max_len);
        }
        else if (in[size - 1] == code_pad) { /* 1 padding char to ignore */
            return b64_to_bin_nopad(in, size - 1, out, max_len);
        }
        else { /* no padding to ignore */
            return b64_to_bin_nopad(in, size, out, max_len);
        }
    }
    else { /* treat as unpadded Base64 */
        return b64_to_bin_nopad(in, size, out, max_len);
    }
}

/* -------------------------------------------------------------------------- */
/* --- STAGE 2: CRC ---------------------- */


void lora_crc16(const char data, int* crc) {
    int next = 0;
    next = (((data >> 0) & 1) ^ ((*crc >> 12) & 1) ^ ((*crc >> 8) & 1));
    next += ((((data >> 1) & 1) ^ ((*crc >> 13) & 1) ^ ((*crc >> 9) & 1)) << 1);
    next += ((((data >> 2) & 1) ^ ((*crc >> 14) & 1) ^ ((*crc >> 10) & 1)) << 2);
    next += ((((data >> 3) & 1) ^ ((*crc >> 15) & 1) ^ ((*crc >> 11) & 1)) << 3);
    next += ((((data >> 4) & 1) ^ ((*crc >> 12) & 1)) << 4);
    next += ((((data >> 5) & 1) ^ ((*crc >> 13) & 1) ^ ((*crc >> 12) & 1) ^ ((*crc >> 8) & 1)) << 5);
    next += ((((data >> 6) & 1) ^ ((*crc >> 14) & 1) ^ ((*crc >> 13) & 1) ^ ((*crc >> 9) & 1)) << 6);
    next += ((((data >> 7) & 1) ^ ((*crc >> 15) & 1) ^ ((*crc >> 14) & 1) ^ ((*crc >> 10) & 1)) << 7);
    next += ((((*crc >> 0) & 1) ^ ((*crc >> 15) & 1) ^ ((*crc >> 11) & 1)) << 8);
    next += ((((*crc >> 1) & 1) ^ ((*crc >> 12) & 1)) << 9);
    next += ((((*crc >> 2) & 1) ^ ((*crc >> 13) & 1)) << 10);
    next += ((((*crc >> 3) & 1) ^ ((*crc >> 14) & 1)) << 11);
    next += ((((*crc >> 4) & 1) ^ ((*crc >> 15) & 1) ^ ((*crc >> 12) & 1) ^ ((*crc >> 8) & 1)) << 12);
    next += ((((*crc >> 5) & 1) ^ ((*crc >> 13) & 1) ^ ((*crc >> 9) & 1)) << 13);
    next += ((((*crc >> 6) & 1) ^ ((*crc >> 14) & 1) ^ ((*crc >> 10) & 1)) << 14);
    next += ((((*crc >> 7) & 1) ^ ((*crc >> 15) & 1) ^ ((*crc >> 11) & 1)) << 15);
    (*crc) = next;
}



uint16_t sx1302_lora_payload_crc(const uint8_t* data, uint8_t size) {
    int i;
    int crc = 0;

    for (i = 0; i < size; i++) {
        lora_crc16(data[i], &crc);
    }

    //printf("CRC16: 0x%02X 0x%02X (%X)\n", (uint8_t)(crc >> 8), (uint8_t)crc, crc);
    return (uint16_t)crc;
}



/* --- EOF ------------------------------------------------------------------ */



int main() {

/* -------------------------------------------------------------------------- */
/* --- STAGE : Decoding ---------------------- */

    uint8_t  payload[256];   /*!> buffer containing the payload */
    const char* str = "QAQTBCaAAwAChqYgr+t4pXpJ"; //从mqtt event里截取
    uint16_t size; //json数据包里自带的，但mqtt event没有

    int i = b64_to_bin(str, strlen(str), payload, sizeof payload); //与net_downlink相似，都是接收到data，故都用b64_to_bin
    
    //if (i != size) {
    //    printf("WARNING: [down] mismatch between .size and .data size once converter to binary\n");
    //}

    size = i;

    printf("PHYPayload: "); //照抄test_loragw_hal_rx里的代码以确定发送的p->payload = PHYPayload
    for (int count = 0; count < size; count++) {
        printf("%02X", payload[count]);
    }
    printf("\n");

    printf("PHYPayload: "); //照抄test_loragw_hal_rx里的代码以确定接收的payload = PHYPayload

    char buff[256] = "";
    char payload1[256] = "";


    //for (uint16_t count = 0; count < size; count++) { //将uint8_t的payload转为char的payload1
    //    
    //    itoa(payload[count], buff, 16);
    //    if (strlen(buff) < 2) {
    //        char o[256] = "0";
    //        strcat(o, buff);
    //        strcpy(buff, o);
    //    }
    //    strcat(payload1, buff);
    //}		

    for (uint16_t count = 0; count < size; count++) {

        sprintf(buff, "%02X", payload[count]); // 大写16进制，宽度占8个位置，左对齐
        strcat(payload1, buff);

    }
    puts(payload1);


    printf("\n");
    printf("SIZE: %d\n", size);


/* -------------------------------------------------------------------------- */
/* --- STAGE : CRC ---------------------- */

    uint16_t    crc;            /*!> CRC that was received in the payload */ //TODO: p->crc
    //uint8_t     payload[256];   /*!> buffer containing the payload */
    uint16_t    payload_crc16_calc;
    payload_crc16_calc = sx1302_lora_payload_crc(payload, size);
    printf("Payload CRC (0x%04X)\n", payload_crc16_calc);




/* -------------------------------------------------------------------------- */
/* --- STAGE : Encoding ---------------------- */


    uint8_t buff_up[16000]="";
    int j = bin_to_b64(payload, size, (char*)(buff_up), 341);
    printf("Data: %s\n", buff_up);

    return 0;
}