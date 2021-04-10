#include "header_1_1.h"
#include "header_1_2.h"
#include "header_1_3.h"
#include "header_1_4.h"

#include "header_2_1.h"
#include "header_2_2.h"

#include "header_3.h"

#include "base64.h"

/* -------------------------------------------------------------------------- */
/* --- DEPENDANCIES --------------------------------------------------------- */




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



void Hex2Bin(char* source, char* dest, int len)
{
    int i = 0;
    char Dict[17][5] =
    {
        "0000",
        "0001",
        "0010",
        "0011",
        "0100",
        "0101",
        "0110",
        "0111",
        "1000",
        "1001",
        "1010",
        "1011",
        "1100",
        "1101",
        "1110",
        "1111",
    };
    for (i = 0; i < len; i++)
    {
        //char temp[5]={0};
        int n = 16;
        if (source[i] >= 'a' && source[i] <= 'f')
            n = source[i] - 'a' + 10;
        if (source[i] >= 'A' && source[i] <= 'F')
            n = source[i] - 'A' + 10;
        if (source[i] >= '0' && source[i] <= '9')
            n = source[i] - '0';
        //sprintf(temp,"%s", Dict[n]);
        //memcpy(&dest[i*4],temp,4);
        memcpy(&dest[i * 4], Dict[n], 4);
    }
    return;
}

int OZ_bin_xor(const char* s1, char* s2, char* dest)
{
    unsigned int i;
    int temp1 = 0, temp2 = 0, temp3 = 0;
    if (strlen(s1) != strlen(s2))
    {
        printf("错误，不等长！\n");
        return 1;
    }
    for (i = 0; i < strlen(s1); i++)
    {
        temp1 = s1[i] - '0';
        temp2 = s2[i] - '0';
        temp3 = temp1 ^ temp2;
        if (temp3 == 1)
            dest[i] = '1';
        else if (temp3 == 0)
            dest[i] = '0';
        else
        {
            printf("字符串内容有误！\n");
            return 1;
        }
    }
    return 0;
}

void Bin2Hex(const char* sSrc, char* sDest, int nSrcLen)
{
    int times = nSrcLen / 4;
    //char temp[times];
    char* temp = new char[times + 1]; //https://blog.csdn.net/weixin_42638401/article/details/88957796

    int x = 0;
    for (int i = 0; i < times; i++)
    {
        //int num=8*int(sSrc[i*4])+4*int(sSrc[i*4+1])+2*int(sSrc[i*4+2])+int(sSrc[i*4+3]);
        x = 8 * (sSrc[i * 4] - '0');
        x += 4 * (sSrc[i * 4 + 1] - '0');
        x += 2 * (sSrc[i * 4 + 2] - '0');
        x += sSrc[i * 4 + 3] - '0';
        sprintf(temp + i, "%1x", x);
    }
    memcpy(sDest, temp, times);

    delete[] temp;
}

void getNe(char* array, int& number) {

    for (unsigned int i = 0; i < strlen(array); i++) {
        if (array[i] == '1') {
            number++;
        }
    }
}

void Uint2Char(uint8_t* array_uint, char* array, int length) {

    char buff[256] = "";

    for (uint16_t count = 0; count < length; count++) {

        sprintf(buff, "%02X", array_uint[count]);
        strcat(array, buff);

    }

}

void Char2Uint(char* array, uint8_t* array_uint, int length) {

    for (int count = 0; count < 2 * length; count++) {
        if (count % 2 == 0) {
            char buff_char[256] = { 0 };
            strncpy(buff_char, array + count, 2); //https://blog.csdn.net/zmhawk/article/details/44600075
            buff_char[strlen(buff_char)] = '\0';
            sscanf(buff_char, "%X", (int*)(&array_uint[count / 2])); //https://bbs.csdn.net/topics/391935459
        }
    }
}

void outmystr(int n, char* input, int compare, char* interoutput, char* finaloutput, int length, int& flag, int& test) //https://bbs.csdn.net/topics/399153127
{
    if (flag == 1) {

        return; //flag=1说明已经有一个crc校验通过的了，直接退出，这样会直接根除掉假阳性false positives (Hash碰撞)

    }

    OZ_bin_xor(input, d, interoutput);

    char Hexstring_temp[64] = { 0 }; //char类型的PHYPayload
    uint8_t  Hexstring_uint8_temp[256] = { 0 };
    uint16_t    payload_crc16_calc_temp = 0;

    Bin2Hex(interoutput, Hexstring_temp, strlen(interoutput));

    Char2Uint(Hexstring_temp, Hexstring_uint8_temp, length);

    payload_crc16_calc_temp = sx1302_lora_payload_crc(Hexstring_uint8_temp, length);

    if (n < 0) {

        if (payload_crc16_calc_temp == compare) {
            strcpy(finaloutput, interoutput);

            flag++;

            /* 测试代码
            printf("Pass crc check time: %d and it happends at candidate number: %d\n", flag, test);
            printf("The Passed InterPHYPayload : ");
            for (int count = 0; count < length; count++) {
                printf("%02X", Hexstring_uint8_temp[count]);
            }
            printf("\n");
            printf("The passed Payload CRC (0x%04X)\n", payload_crc16_calc_temp);
            */

            //TODO: hidden errors
            //TODO: 超时退出程序：//https://blog.csdn.net/codedz/article/details/80387001, 其实可以不用，最大纠错比特位数量Hamming_weight_max已经可以用来限制了
        }

        /* 测试代码
        printf("Candidate: %s\n", d);
        printf("Interoutput: %s\n", interoutput);
        */

        /* 测试代码
        test++;

        printf("The number of candidate: %d\n", test);
        printf("Every InterPHYPayload: \n");
        for (int count = 0; count < length; count++) {
            printf("%02X", Hexstring_uint8_temp[count]);
        }
        printf("Every Payload CRC (0x%04X)\n", payload_crc16_calc_temp);
        printf("\n");
        */
    }
    else
    {
        d[n] = '0';
        outmystr(n - 1, input, compare, interoutput, finaloutput, length, flag, test);
        if (s[n] == '1')
        {
            d[n] = '1';
            outmystr(n - 1, input, compare, interoutput, finaloutput, length, flag, test);
        }

    }

}

int FindSubchar(char* fullchar, char* subchar) {

    char* buffer; //用于接受返回值

    if ((buffer = strstr(fullchar, subchar)) == NULL)
    { //说明没有要找的字符串
        return -1;
    }
    else
    {                                 //说明找到了那个字符串
        return buffer - fullchar + 1; //cde的地址减去abcde的地址+1
    }

}

void getStat(char* char1, char* char2, char* char3) {

    strncpy(char1, char2 + FindSubchar(char2, char3) + 5, 1); //https://blog.csdn.net/zmhawk/article/details/44600075
}

void getCrc(char* char1, char* char2, char* char3, char* char4) {

    strncpy(char1, char2 + FindSubchar(char2, char3) + 4, FindSubchar(char2, char4) - FindSubchar(char2, char3) - 7); //https://blog.csdn.net/zmhawk/article/details/44600075
}


void getStr(char* char1, char* char2, char* char3, int num) {

    strncpy(char1, char2 + FindSubchar(char2, char3) + 6, num - FindSubchar(char2, char3)-22); //https://blog.csdn.net/zmhawk/article/details/44600075

}

void getRssis(char* char1, char* char2, char* char3, char* char4) {
   strncpy(char1, char2 + FindSubchar(char2, char3) + 6 , FindSubchar(char2, char4) - FindSubchar(char2, char3) - 9 );
}



int main()

{
    //TODO: 取多个真实值实验
    char buffer1[BUF_SIZE] = "027CC2000016C001FF10D3F67B227278706B223A5B7B226A766572223A312C22746D7374223A34383939353534302C2274696D65223A22323032312D30342D31305431313A35343A32372E3030303030303030305A222C22746D6D73223A313330323039303836373030302C226368616E223A352C2272666368223A312C2266726571223A3438372E3330303030302C226D6964223A20382C2273746174223A312C22637263223A35333733302C226D6F6475223A224C4F5241222C2264617472223A225346374257313235222C22636F6472223A22342F35222C227273736973223A2D372C226C736E72223A31332E322C22666F6666223A2D3237372C2272737369223A2D362C2273697A65223A32382C2264617461223A225141515442436141437741434D64615748495237324366567577377739454A675055415873513D3D227D5D7D";
    char buffer2[BUF_SIZE] = "027CC2000016C001FF10D3F67B227278706B223A5B7B226A766572223A312C22746D7374223A34383939353534302C2274696D65223A22323032312D30342D31305431313A35343A32372E3030303030303030305A222C22746D6D73223A313330323039303836373030302C226368616E223A352C2272666368223A312C2266726571223A3438372E3330303030302C226D6964223A20382C2273746174223A312C22637263223A35333733302C226D6F6475223A224C4F5241222C2264617472223A225346374257313235222C22636F6472223A22342F35222C227273736973223A2D372C226C736E72223A31332E322C22666F6666223A2D3237372C2272737369223A2D362C2273697A65223A32382C2264617461223A225141515442436141437741434D64615748495237324366567577377739454A675055415873513D3D227D5D7D";
    //TODO: 扩容程序（数组），目前最大实验数据仅到15byte

    int buff_index1 = strlen(buffer1) / 2;
    int buff_index2 = strlen(buffer2) / 2;

    uint8_t  buffer_uint1[BUF_SIZE] = "";
    uint8_t  buffer_uint2[BUF_SIZE] = "";
    Char2Uint(buffer1, buffer_uint1, buff_index1);
    Char2Uint(buffer2, buffer_uint2, buff_index2);



    char* buffer1_inter = (char*)(buffer_uint1 + 12);
    char* buffer2_inter = (char*)(buffer_uint2 + 12);
    printf("buffer1_inter: %s\n", buffer1_inter);
    printf("\n");
    printf("buffer2_inter: %s\n", buffer2_inter);
    printf("\n");


    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 找到上行数据中需要的属性的值 ---------------------- */
    //TODO: 解决多数据包同时上行情况 (重复数据包接收是因为距离太近)(https://forum.rakwireless.com/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/3980/3?u=haowong)

    char report1[BUFSIZ] = "stat";
    char report2[BUFSIZ] = "crc";
    char report3[BUFSIZ] = "modu";
    char report4[BUFSIZ] = "data";
    char report5[BUFSIZ] = "rssis";
    char report6[BUFSIZ] = "lsnr";
    

    char stat1[BUFSIZ] = { 0 };
    getStat(stat1, buffer1_inter, report1);
    char stat2[BUFSIZ] = { 0 };
    getStat(stat2, buffer2_inter, report1);

    char crc_get1[BUFSIZ] = { 0 };
    getCrc(crc_get1, buffer1_inter, report2, report3);
    char crc_get2[BUFSIZ] = { 0 };
    getCrc(crc_get2, buffer2_inter, report2, report3);
    char crc_get[BUFSIZ] = { 0 };

    char str1[BUFSIZ] = { 0 };
    getStr(str1, buffer1_inter, report4, buff_index1);
    char str2[BUFSIZ] = { 0 };
    getStr(str2, buffer2_inter, report4, buff_index2);

    char rssis1[BUFSIZ] = { 0 };
    getRssis(rssis1, buffer1_inter, report5, report6);
    char rssis2[BUFSIZ] = { 0 };
    getRssis(rssis2, buffer2_inter, report5, report6);

    /*测试代码
    printf("stat1: %s\n", stat1);
    printf("crc_get1: %s\n", crc_get1);
    printf("str1: %s\n", str1);
    printf("rssis1: %s\n", rssis1);
    */


    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 当两个上行数据都错且crc值相同时进行纠错 ---------------------- */
    
    
    if ((strcmp(stat1, "1") == 0) && (strcmp(stat2, "1") == 0)) { //TODO: 当使用真实值时把1都改为-1

        if (strcmp(crc_get1, crc_get2) == 0) {

            strcpy(crc_get, crc_get1);

            /* -------------------------------------------------------------------------- */
            /* --- STAGE : Decoding ---------------------- */


            uint8_t  payload1[256];   /*!> buffer containing the payload */
            uint16_t size1; //json数据包里自带的，但mqtt event没有
            size1 = b64_to_bin(str1, strlen(str1), payload1, sizeof payload1); //与net_downlink相似，都是接收到data，故都用b64_to_bin
            printf("InputData1: %s\n", str1);


            uint8_t  payload2[256];   /*!> buffer containing the payload */
            uint16_t size2; //json数据包里自带的，但mqtt event没有
            size2 = b64_to_bin(str2, strlen(str2), payload2, sizeof payload2); //与net_downlink相似，都是接收到data，故都用b64_to_bin
            printf("InputData2: %s\n", str2);


            uint16_t size;

            if (size1 == size2) {
                size = size1;
            }
            else {
                printf("Error: length1 is not equal to length2. This program will be shut down!");
                return 0;
            }


            /* -------------------------------------------------------------------------- */
            /* --- STAGE : uint8_t转char ---------------------- */ //https://bbs.csdn.net/topics/390141308


            char Hexstring1[256] = "";
            Uint2Char(payload1, Hexstring1, size);
            /* 测试代码
            printf("M's: %s\n", Hexstring1);
            */

            char Hexstring2[256] = "";
            Uint2Char(payload2, Hexstring2, size);
            /* 测试代码
            printf("M'r: %s\n", Hexstring2);
            */


            /* -------------------------------------------------------------------------- */
            /* --- STAGE : 十六进制字符串转二进制字符串 ---------------------- */ //https://blog.csdn.net/weixin_30279751/article/details/95437814

            char Binarystring1[256] = "";
            char Binarystring2[256] = "";

            Hex2Bin(Hexstring1, Binarystring1, strlen(Hexstring1));
            Hex2Bin(Hexstring2, Binarystring2, strlen(Hexstring2));


            /* -------------------------------------------------------------------------- */
            /* --- STAGE : 二进制字符串异或 ---------------------- */


            char Binarystring3[256] = ""; ////Merged error mask / Ambiguity vectors / Va

            if (OZ_bin_xor(Binarystring1, Binarystring2, Binarystring3) != 0) //TODO: Majority voting / more than two copies
            {
                printf("函数出错！\n");
                return 1;
            }

            /* -------------------------------------------------------------------------- */
            /* --- STAGE : GetCandidate ---------------------- */
            /* -------------------------------------------------------------------------- */
            /* --- STAGE : CRC ---------------------- */


            char mch[256] = "";
            if (rssis1 > rssis2) {

                strcpy(mch, Binarystring1);

            }
            else {

                strcpy(mch, Binarystring2);

            }
            /* 测试代码
             printf("MCH: %s\n", mch);
             */
            unsigned int crc_buffer = atoi(crc_get);
            char crc[256] = "";
            sprintf(crc, "0x%04X", crc_buffer);
            printf("Processed CRC: %s\n", crc);
            int crc_int = 0;
            sscanf(crc, "%X", &crc_int); //用sscanf而不是atoi的原因是linux没有atoi，但是crc最前面的0还是没了
            /* 测试代码
            printf("CRC int: %x\n", crc_int);
            */
            int i = 0;
            strcpy(s, Binarystring3);
            /* 测试代码
            printf("Mask: %s\n", s);
            */

            int Hamming_weight_now = 0;
            int Hamming_weight_max = 30; //预设的最多纠错比特位数量
            getNe(s, Hamming_weight_now);
            if (Hamming_weight_now > Hamming_weight_max) {

                printf("%s: %d\n", "Hamming weight is larger than the max number", Hamming_weight_max);
                printf("This program will be shut down!\n");
                return 0;

            } {
                /* 测试代码
                printf("Hamming Weight: %d\n", Hamming_weight_now);
                */
            }

            char fakeresult[256] = ""; //每次candidate与mch异或的中间产值
            char realresult[256] = ""; //符合CRC校验的fakeresult
            int total_number = 0; //一共运行的次数
            int pass_crc = 0; //符合CRC校验的次数


            while (s[i])
                d[i++] = '0';

            outmystr(i - 1, mch, crc_int, fakeresult, realresult, size, pass_crc, total_number); //TODO: 按照hamming weight递增的方法产生error candidate

            if (strlen(realresult) == 0) {
                printf("%s\n", "Error can not be fixed! This program will be shut down!");
                //CRC未出错的话一定出现了hidden error
                return 0;
            }

            /* 测试代码
            printf("RealresultBit: %s\n", realresult);
            */

            /* 测试代码：须关闭 if(flag == 1)判断条件，否则永远不会出现假阳性
            if (pass_crc > 1){

                printf("%s\n", "Falsepositive happens");
            }
            */


            /* -------------------------------------------------------------------------- */
            /* --- STAGE : 二进制字符串转十六进制字符串 ---------------------- */


            char Hexstring4[64] = { 0 }; //char类型的PHYPayload

            Bin2Hex(realresult, Hexstring4, strlen(realresult));
            /* 测试代码
            printf("RealresultHex: %s\n", Hexstring4);
            */

            /* -------------------------------------------------------------------------- */
            /* --- STAGE : Encoding ---------------------- */

            uint8_t  Hexstring4_uint8[256] = "";

            Char2Uint(Hexstring4, Hexstring4_uint8, size);

            uint8_t data_up_uint8[10000] = ""; //不用太大， 因为原代码里的buff_up不止装的data所以很大
            bin_to_b64(Hexstring4_uint8, size, (char*)(data_up_uint8), 341);

            char data_up[256] = "";
            strcpy(data_up, (char*)(data_up_uint8));
            printf("OutputData: %s\n", data_up);

            /* 测试代码
            uint16_t    payload_crc16_calc;
            payload_crc16_calc = sx1302_lora_payload_crc(Hexstring4_uint8, size);
            printf("FixedPayload CRC (0x%04X)\n", payload_crc16_calc);
            */


            /* -------------------------------------------------------------------------- */
            /* --- STAGE : 替换data ---------------------- */
            //TODO: 解决多数据包同时上行情况
            
            char buffer_inter[BUFSIZ] = { 0 };
            char buffer_inter_uint_char[1250] = { 0 };

            char buffer_send_first_half_char[1250] = { 0 };
            char buffer_send_last_half_char[1250] = { 0 };
            uint8_t buffer_send[1250];


            if (rssis1 > rssis2) {


                /* -------------------------------------------------------------------------- */
                /* --- STAGE : 构造出前24个字符缺陷的buffer_inter_uint_char ---------------------- */
                
                
                strncpy(buffer1_inter + FindSubchar(buffer1_inter,report4) + 6, data_up, strlen(data_up)); //https://blog.csdn.net/zmhawk/article/details/44600075
                strcpy(buffer_inter, buffer1_inter);
                uint8_t* buffer_inter_uint = (uint8_t*)(buffer_inter - 12);
                Uint2Char(buffer_inter_uint, buffer_inter_uint_char, buff_index1);

                /* -------------------------------------------------------------------------- */
                /* --- STAGE : 将buff_i的前二十四个字符(必然不会被修改的部分) 与buffer_inter_uint_char的二十四个字符开始的部分(修改了的部分) 组合起来，转换为uint8_t ---------------------- */
                
                
                strncpy(buffer_send_first_half_char, buffer1, 24);
                buffer_send_first_half_char[strlen(buffer_send_first_half_char)] = '\0';
                strncpy(buffer_send_last_half_char, buffer_inter_uint_char + 24, strlen(buffer_inter_uint_char) - 24);
                buffer_send_last_half_char[strlen(buffer_send_last_half_char)] = '\0';

                strcat(buffer_send_first_half_char, buffer_send_last_half_char);
                Char2Uint(buffer_send_first_half_char, buffer_send, buff_index1);

                printf("buffer_send: ");
                for (int count = 0; count < buff_index1; count++) {
                    printf("%02X", buffer_send[count]);
                }
 
            }
            else {

                strncpy(buffer2_inter + FindSubchar(buffer2_inter, report4) + 6, data_up, strlen(data_up));
                strcpy(buffer_inter, buffer2_inter);
                uint8_t* buffer_inter_uint = (uint8_t*)(buffer_inter - 12);
                Uint2Char(buffer_inter_uint, buffer_inter_uint_char, buff_index2);


                strncpy(buffer_send_first_half_char, buffer2, 24);
                buffer_send_first_half_char[strlen(buffer_send_first_half_char)] = '\0';
                strncpy(buffer_send_last_half_char, buffer_inter_uint_char + 24, strlen(buffer_inter_uint_char) - 24);
                buffer_send_last_half_char[strlen(buffer_send_last_half_char)] = '\0';

                strcat(buffer_send_first_half_char, buffer_send_last_half_char);
                Char2Uint(buffer_send_first_half_char, buffer_send, buff_index2);

                printf("buffer_send: ");
                for (int count = 0; count < buff_index2; count++) {
                    printf("%02X", buffer_send[count]);
                }


            }


            




        }

    }


    return 0;
}