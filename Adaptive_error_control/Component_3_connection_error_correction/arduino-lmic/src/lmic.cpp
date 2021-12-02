//
// Created by 229077035 on 2021/12/2.
//
#include "../inc/lmic.h"


// generate 1+10 roundkeys for encryption with 128-bit key
// read 128-bit key from AESKEY in MSBF, generate roundkey words in place
static void aesroundkeys () {
    int i;
    u4_t b;

    for( i=0; i<4; i++) {
        AESKEY[i] = swapmsbf(AESKEY[i]);
    }

    b = AESKEY[3];
    for( ; i<44; i++ ) {
        if( i%4==0 ) {
            // b = SubWord(RotWord(b)) xor Rcon[i/4]
            b = ((u4_t)TABLE_GET_U1(AES_S, u1(b >> 16)) << 24) ^
                ((u4_t)TABLE_GET_U1(AES_S, u1(b >>  8)) << 16) ^
                ((u4_t)TABLE_GET_U1(AES_S, u1(b)      ) <<  8) ^
                ((u4_t)TABLE_GET_U1(AES_S,    b >> 24 )      ) ^
                TABLE_GET_U4(AES_RCON, (i-4)/4);
        }
        AESKEY[i] = b ^= AESKEY[i-4];
    }
}


u4_t os_aes (u1_t mode, xref2u1_t buf, u2_t len) {

    aesroundkeys();

    if( mode & AES_MICNOAUX ) {
        AESAUX[0] = AESAUX[1] = AESAUX[2] = AESAUX[3] = 0;
    } else {
        AESAUX[0] = swapmsbf(AESAUX[0]);
        AESAUX[1] = swapmsbf(AESAUX[1]);
        AESAUX[2] = swapmsbf(AESAUX[2]);
        AESAUX[3] = swapmsbf(AESAUX[3]);
    }

    while( (signed char)len > 0 ) {
        u4_t a0, a1, a2, a3;
        u4_t t0, t1, t2, t3;
        u4_t *ki, *ke;

        // load input block
        if( (mode & AES_CTR) || ((mode & AES_MIC) && (mode & AES_MICNOAUX)==0) ) { // load CTR block or first MIC block
            a0 = AESAUX[0];
            a1 = AESAUX[1];
            a2 = AESAUX[2];
            a3 = AESAUX[3];
        }
        else if( (mode & AES_MIC) && len <= 16 ) { // last MIC block
            a0 = a1 = a2 = a3 = 0; // load null block
            mode |= ((len == 16) ? 1 : 2) << 4; // set MICSUB: CMAC subkey K1 or K2
        } else
        LOADDATA: { // load data block (partially)
            for(t0=0; t0<16; t0++) {
                t1 = (t1<<8) | ((t0<len) ? buf[t0] : (t0==len) ? 0x80 : 0x00);
                if((t0&3)==3) {
                    a0 = a1;
                    a1 = a2;
                    a2 = a3;
                    a3 = t1;
                }
            }
            if( mode & AES_MIC ) {
                a0 ^= AESAUX[0];
                a1 ^= AESAUX[1];
                a2 ^= AESAUX[2];
                a3 ^= AESAUX[3];
            }
        }

            // perform AES encryption on block in a0-a3
            ki = AESKEY;
        ke = ki + 8*4;
        a0 ^= ki[0];
        a1 ^= ki[1];
        a2 ^= ki[2];
        a3 ^= ki[3];
        do {
            AES_key4 (t1,t2,t3,t0,4);
            AES_expr4(t1,t2,t3,t0,a0);
            AES_expr4(t2,t3,t0,t1,a1);
            AES_expr4(t3,t0,t1,t2,a2);
            AES_expr4(t0,t1,t2,t3,a3);

            AES_key4 (a1,a2,a3,a0,8);
            AES_expr4(a1,a2,a3,a0,t0);
            AES_expr4(a2,a3,a0,a1,t1);
            AES_expr4(a3,a0,a1,a2,t2);
            AES_expr4(a0,a1,a2,a3,t3);
        } while( (ki+=8) < ke );

        AES_key4 (t1,t2,t3,t0,4);
        AES_expr4(t1,t2,t3,t0,a0);
        AES_expr4(t2,t3,t0,t1,a1);
        AES_expr4(t3,t0,t1,t2,a2);
        AES_expr4(t0,t1,t2,t3,a3);

        AES_expr(a0,t0,t1,t2,t3,8);
        AES_expr(a1,t1,t2,t3,t0,9);
        AES_expr(a2,t2,t3,t0,t1,10);
        AES_expr(a3,t3,t0,t1,t2,11);
        // result of AES encryption in a0-a3

        if( mode & AES_MIC ) {
            if( (t1 = (mode & AES_MICSUB) >> 4) != 0 ) { // last block
                do {
                    // compute CMAC subkey K1 and K2
                    t0 = a0 >> 31; // save MSB
                    a0 = (a0 << 1) | (a1 >> 31);
                    a1 = (a1 << 1) | (a2 >> 31);
                    a2 = (a2 << 1) | (a3 >> 31);
                    a3 = (a3 << 1);
                    if( t0 ) a3 ^= 0x87;
                } while( --t1 );

                AESAUX[0] ^= a0;
                AESAUX[1] ^= a1;
                AESAUX[2] ^= a2;
                AESAUX[3] ^= a3;
                mode &= ~AES_MICSUB;
                goto LOADDATA;
            } else {
                // save cipher block as new iv
                AESAUX[0] = a0;
                AESAUX[1] = a1;
                AESAUX[2] = a2;
                AESAUX[3] = a3;
            }
        } else { // CIPHER
            if( mode & AES_CTR ) { // xor block (partially)
                t0 = (len > 16) ? 16: len;
                for(t1=0; t1<t0; t1++) {
                    buf[t1] ^= (a0>>24);
                    a0 <<= 8;
                    if((t1&3)==3) {
                        a0 = a1;
                        a1 = a2;
                        a2 = a3;
                    }
                }
                // update counter
                AESAUX[3]++;
            } else { // ECB
                // store block
                msbf4_write(buf+0,  a0);
                msbf4_write(buf+4,  a1);
                msbf4_write(buf+8,  a2);
                msbf4_write(buf+12, a3);
            }
        }

        // update block state
        if( (mode & AES_MIC)==0 || (mode & AES_MICNOAUX) ) {
            buf += 16;
            len -= 16;
        }
        mode |= AES_MICNOAUX;
    }
    return AESAUX[0];
}

#if !defined(os_rmsbf4)
u4_t os_rmsbf4 (xref2cu1_t buf) {
    return (u4_t)((u4_t)buf[3] | ((u4_t)buf[2]<<8) | ((u4_t)buf[1]<<16) | ((u4_t)buf[0]<<24));
}
#endif

void os_wlsbf4 (xref2u1_t buf, u4_t v) {
    buf[0] = v;
    buf[1] = v>>8;
    buf[2] = v>>16;
    buf[3] = v>>24;
}

void os_wmsbf4 (xref2u1_t buf, u4_t v) {
    buf[3] = v;
    buf[2] = v>>8;
    buf[1] = v>>16;
    buf[0] = v>>24;
}

void micB0 (u4_t devaddr, u4_t seqno, int dndir, int len) {
    os_clearMem(AESaux,16);
    AESaux[0]  = 0x49;
    AESaux[5]  = dndir?1:0;
    AESaux[15] = len;
    os_wlsbf4(AESaux+ 6,devaddr);
    os_wlsbf4(AESaux+10,seqno);
}

int aes_appendMic (xref2cu1_t key, u4_t devaddr, u4_t seqno, int dndir, xref2u1_t pdu, int len) {
    micB0(devaddr, seqno, dndir, len);
    os_copyMem(AESkey,key,16);
    // MSB because of internal structure of AES

    u4_t v = os_aes(AES_MIC, pdu, len);
    xref2u1_t buf;
    buf[3] = v;
    buf[2] = v>>8;
    buf[1] = v>>16;
    buf[0] = v>>24;

    if((buf[0]==pdu[len]) && (buf[1]==pdu[len+1]) && (buf[2]==pdu[len+2]) && (buf[3]==pdu[len+3])){
        printf("MIC: ");
        for(int count = 0; count < 4; count++){
            printf("%02X", buf[count]);
        }
        printf("\n");

        return 1;
    }

    return 0;

}