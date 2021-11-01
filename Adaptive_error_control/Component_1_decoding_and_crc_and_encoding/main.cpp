#include "payload_crc.h"

#include "base64.h"


int main() {

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : Decoding ---------------------- */

    uint8_t payload[BUF_SIZE];                                               /*!> buffer containing the payload */
    const char *str = "QAQTBCaAAAACMkUdJTwAKNpcr9d7VEkbqZW9xilJAImfRhwIXIg=";//从mqtt event里截取
    uint16_t size;                                                           //json数据包里自带的，但mqtt event没有

    int i = b64_to_bin(str, strlen(str), payload, sizeof payload);//与net_downlink相似，都是接收到data，故都用b64_to_bin

    //if (i != size) {
    //    printf("WARNING: [down] mismatch between .size and .data size once converter to binary\n");
    //}

    size = i;

    printf("PHYPayload: ");//照抄test_loragw_hal_rx里的代码以确定发送的p->payload = PHYPayload
    for (int count = 0; count < size; count++) {
        printf("%02X", payload[count]);
    }
    printf("\n");

    printf("PHYPayload: ");//照抄test_loragw_hal_rx里的代码以确定接收的payload = PHYPayload

    char buff[BUF_SIZE] = {0};
    char payload1[BUF_SIZE] = {0};


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

        sprintf(buff, "%02X", payload[count]);// 大写16进制，宽度占8个位置，左对齐
        strcat(payload1, buff);
    }
    puts(payload1);


    printf("\n");
    printf("SIZE: %d\n", size);


    /* -------------------------------------------------------------------------- */
    /* --- STAGE : CRC ---------------------- */

    uint16_t crc; /*!> CRC that was received in the payload */
    //uint8_t     payload[256];   /*!> buffer containing the payload */
    uint16_t payload_crc16_calc;
    payload_crc16_calc = sx1302_lora_payload_crc(payload, size);
    printf("Payload CRC (0x%04X)\n", payload_crc16_calc);


    /* -------------------------------------------------------------------------- */
    /* --- STAGE : Encoding ---------------------- */


    uint8_t buff_up[BUF_SIZE] = {0};
    int j = bin_to_b64(payload, size, (char *) (buff_up), 341);
    printf("Data: %s\n", buff_up);

    return 0;
}