#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <stdio.h>
#include <algorithm>
using namespace std;

void lora_crc16_copy(const char data, int *crc) {
    int next = 0;
    next  =  (((data>>0)&1) ^ ((*crc>>12)&1) ^ ((*crc>> 8)&1)                 )      ;
    next += ((((data>>1)&1) ^ ((*crc>>13)&1) ^ ((*crc>> 9)&1)                 )<<1 ) ;
    next += ((((data>>2)&1) ^ ((*crc>>14)&1) ^ ((*crc>>10)&1)                 )<<2 ) ;
    next += ((((data>>3)&1) ^ ((*crc>>15)&1) ^ ((*crc>>11)&1)                 )<<3 ) ;
    next += ((((data>>4)&1) ^ ((*crc>>12)&1)                                  )<<4 ) ;
    next += ((((data>>5)&1) ^ ((*crc>>13)&1) ^ ((*crc>>12)&1) ^ ((*crc>> 8)&1))<<5 ) ;
    next += ((((data>>6)&1) ^ ((*crc>>14)&1) ^ ((*crc>>13)&1) ^ ((*crc>> 9)&1))<<6 ) ;
    next += ((((data>>7)&1) ^ ((*crc>>15)&1) ^ ((*crc>>14)&1) ^ ((*crc>>10)&1))<<7 ) ;
    next += ((((*crc>>0)&1) ^ ((*crc>>15)&1) ^ ((*crc>>11)&1)                 )<<8 ) ;
    next += ((((*crc>>1)&1) ^ ((*crc>>12)&1)                                  )<<9 ) ;
    next += ((((*crc>>2)&1) ^ ((*crc>>13)&1)                                  )<<10) ;
    next += ((((*crc>>3)&1) ^ ((*crc>>14)&1)                                  )<<11) ;
    next += ((((*crc>>4)&1) ^ ((*crc>>15)&1) ^ ((*crc>>12)&1) ^ ((*crc>> 8)&1))<<12) ;
    next += ((((*crc>>5)&1) ^ ((*crc>>13)&1) ^ ((*crc>> 9)&1)                 )<<13) ;
    next += ((((*crc>>6)&1) ^ ((*crc>>14)&1) ^ ((*crc>>10)&1)                 )<<14) ;
    next += ((((*crc>>7)&1) ^ ((*crc>>15)&1) ^ ((*crc>>11)&1)                 )<<15) ;
    (*crc) = next;
}



uint16_t sx1302_lora_payload_crc_copy(const uint8_t * data, uint8_t size) {
    int i;
    int crc = 0;

    for (i = 0; i < size; i++) {
        lora_crc16_copy(data[i], &crc);
    }

    //printf("CRC16: 0x%02X 0x%02X (%X)\n", (uint8_t)(crc >> 8), (uint8_t)crc, crc);
    return (uint16_t)crc;
}



int main()
{
    /*
	//uint8_t pkt_data[]="410C205013000100010048656C6C6F";
    uint8_t data[]="4004130426801C0002FF4E6F863140784B45";
    uint16_t size;
    uint8_t pkt_rxbytenb_modem = sizeof(data)/2;
    size = pkt_rxbytenb_modem;
    //memcpy((void *)data, (void *)(&(pkt_data)), pkt_rxbytenb_modem);
    uint16_t payload_crc16_calc;
    cout<<size<<endl;   
    payload_crc16_calc = sx1302_lora_payload_crc_copy(data, size);
    printf("Payload CRC16 (calc:0x%04X)\n", payload_crc16_calc); 
    //printf("CRC16: 0x%02X 0x%02X (%X)\n", (uint8_t)(crc >> 8), (uint8_t)crc, crc);
    printf("crc: 0x%04X\n", (uint16_t)payload_crc16_calc);
	//cout<<(uint16_t)crc;
    */
   
    uint16_t    crc;            /*!> CRC that was received in the payload */
    uint16_t    size;           /*!> payload size in bytes */
    uint8_t     payload[256];   /*!> buffer containing the payload */
    uint16_t    payload_crc16_calc;
    payload = "4004130426800900025746F11BE68D273364";
    payload_crc16_calc = sx1302_lora_payload_crc_copy(payload, 18);
    if (payload_crc16_calc != crc) {
        printf("ERROR: Payload CRC16 check failed (got:0x%04X calc:0x%04X)\n", crc, payload_crc16_calc); //p->crc是由直接得到的pkt.rx_crc16_value赋值而来 (如果crc16计算失败将放弃赋值：只有pkt.payload_crc_error=0才进行健全性计算检查)
    } else {
        printf("Payload CRC check OK (0x%04X)\n", crc);
    }

}
