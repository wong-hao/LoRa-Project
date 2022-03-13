#include "header_1_1.h"
#include "header_1_2.h"
#include "header_1_3.h"

#include "header_2_1.h"
#include "header_2_2.h"

#include "payload_diff.h"

#include "header_3.h"

#include "base64.h"
#include "timelib.h"

int main()

{
    printf("Algorithm parameters: \n");
    printf("{\n    BUF_SIZE: %d\n", BUF_SIZE);
    printf("    MAXLATENCY: %f\n", MAXLATENCY);
    printf("    GW num: %d\n", GW);
    printf("    StageOption: %d\n}\n", StageOption);

    if (Concurrent != 1) {
        printf("    Concurrent: %d (Should be '1'), the program will be shut down\n", Concurrent);
        return 0;
    }
    printf("The error control server (port: %d) waits for connections and forward to Network server (address: %s, port: %s)!\n\n", 1680, serv_addr, serv_port_up);

    Buffer buffer_array[GW];

    BufferSend buffer{};

    for (int loopcount = 0; loopcount <= GW - 1; loopcount++) {
        buffer_array[loopcount].data = new char[BUF_SIZE];
        memset(buffer_array[loopcount].data, 0, BUF_SIZE * sizeof(char));
    }

    initFile();
    initErrorFile();

    double throughoutData = 0;//PHY Payload
    double throughout = 0;

    double CRCErrorNumBefore = 0;
    double NonCRCErrorNumBefore = 0;
    double PERBefore;
    double PDRBefore;

    bool CorrectedFlag = false; //防止纠错不成功后仍使得NonCRCErrorNumAfter错误增加
    double CRCErrorNumAfter = 0;//计算无论经过纠错或未经过，最终未通过CRC校验的次数
    double NonCRCErrorNumAfter = 0;
    double PERAfter;//计算无论经过纠错或未经过，最终未通过CRC校验的全局PER
    double PDRAfter;

    while (true) {

        /* -------------------------------------------------------------------------- */
        /* --- STAGE : Socket接收到数据---------------------- */

        char **buff_up_char = new char *[GW];

        for (int i = 0; i <= GW - 1; i++) {
            buff_up_char[i] = new char[BUF_SIZE]();
            memset(buff_up_char[i], 0, BUF_SIZE * sizeof(char));
        }

        switch (GW) {
            case 4:{
                strcpy(buff_up_char[3], "02E506000016C001FF10D4467B227278706B223A5B7B226A766572223A312C22746D7374223A3333333332373037352C2274696D65223A22323032312D31312D32305431313A33383A31362E3030303030303030305A222C22746D6D73223A313332313434333439363030302C226368616E223A362C2272666368223A312C2266726571223A3438372E3530303030302C226D6964223A20382C2273746174223A2D312C226D6F6475223A224C4F5241222C2264617472223A22534631304257313235222C22636F6472223A22342F35222C227273736973223A2D35362C226C736E72223A2D31302E302C22666F6666223A313637332C22637263223A31393637392C2244657641646472223A31343630383931372C2266636E74223A3130342C2272737369223A2D35372C2273697A65223A35362C2264617461223A225142587133674141614141422B574D535252534374424E68326D4A455646594E353878496253744B705A456E4662717A506B77356659337745316A55476A744D51685652736A4E345463413D227D5D7D");
            }
            case 3:{
                strcpy(buff_up_char[2], "02A844000016C001FF10D42D7B227278706B223A5B7B226A766572223A312C22746D7374223A3333313638333834372C2274696D65223A22323032312D31312D32305431313A33383A31362E3030303030303030305A222C22746D6D73223A313332313434333439363030302C226368616E223A362C2272666368223A312C2266726571223A3438372E3530303030302C226D6964223A20382C2273746174223A2D312C226D6F6475223A224C4F5241222C2264617472223A22534631304257313235222C22636F6472223A22342F35222C227273736973223A2D36332C226C736E72223A302E352C22666F6666223A313539372C22637263223A31393637392C2244657641646472223A31343630383931372C2266636E74223A3130342C2272737369223A2D36322C2273697A65223A35362C2264617461223A225142587133674141614141422B574D535252534374424E68326D4A455646594E3538784962537371705A456E4662717A506B77356659337745316A55476A744D51705642736D4F6F6662493D227D5D7D");
            }
            case 2:{
                strcpy(buff_up_char[0], "02D4B2000016C001FF10D3F67B227278706B223A5B7B226A766572223A312C22746D7374223A3332363132363933392C2274696D65223A22323032312D31312D32305431313A33383A31362E3030303030303030305A222C22746D6D73223A313332313434333439363030302C226368616E223A362C2272666368223A312C2266726571223A3438372E3530303030302C226D6964223A20382C2273746174223A2D312C226D6F6475223A224C4F5241222C2264617472223A22534631304257313235222C22636F6472223A22342F35222C227273736973223A2D37372C226C736E72223A312E382C22666F6666223A313636352C22637263223A31393637392C2244657641646472223A31343630383931372C2266636E74223A3130342C2272737369223A2D37392C2273697A65223A35362C2264617461223A225142587133674141614141422B574D535252534374424E68326D4A455646594E3538784962537371705A456E4662717A506B77356659337745316A55476A744D51705642736D4F6F6666493D227D5D7D");
            }
            case 1:{
                strcpy(buff_up_char[1], "02AA1A000016C001FF10D3F77B227278706B223A5B7B226A766572223A312C22746D7374223A3333303237333334352C2274696D65223A22323032312D31312D32305431313A33383A31362E3030303030303030305A222C22746D6D73223A313332313434333439363030302C226368616E223A362C2272666368223A312C2266726571223A3438372E3530303030302C226D6964223A20382C2273746174223A2D312C226D6F6475223A224C4F5241222C2264617472223A22534631304257313235222C22636F6472223A22342F35222C227273736973223A2D38362C226C736E72223A2D342E382C22666F6666223A313634322C22637263223A31393637392C2244657641646472223A31343630383931372C2266636E74223A3130342C2272737369223A2D38332C2273697A65223A35362C2264617461223A225142587133674141614141422B574D535252534374424E68326D4A455646594E3538784962537371705A456E4662717A506B77356659337745316A55476A744D51705642736D4F6F6666493D227D5D7D");
            }
                break;
            default:{
                printf("GW num is illegal!\n");
                return 0;
            }
        }

        for (int i = 0; i <= GW - 1; i++) {
            buffer_array[i].setData(buff_up_char[i]);
        }

        delete[] buff_up_char;

        /* -------------------------------------------------------------------------- */
        /* --- STAGE : deal with the message---------------------- */

        for (int loopcount = 0; loopcount <= GW - 1; loopcount++) {
            buffer_array[loopcount].setIndex();
            memset(buffer_array[loopcount].uint, 0, BUF_SIZE * sizeof(uint8_t));
            buffer_array[loopcount].setUint();
        }

        /* -------------------------------------------------------------------------- */
        /* --- STAGE : 对中间数据buffer_inter纠错---------------------- */

        for (int loopcount = 0; loopcount <= GW - 1; loopcount++) {
            buffer_array[loopcount].inter = new char[BUF_SIZE];
            memset(buffer_array[loopcount].inter, 0, BUF_SIZE * sizeof(char));

            buffer_array[loopcount].setInter();//接收到的Upstream JSON data structure

            buffer_array[loopcount].inter_uint = new uint8_t[BUF_SIZE];
            memset(buffer_array[loopcount].inter_uint, 0, BUF_SIZE * sizeof(uint8_t));
            cout << "buffer" << loopcount + 1 << ".inter: " << buffer_array[loopcount].inter << endl;
            buffer_array[loopcount].setInter_Uint();
        }

        /* -------------------------------------------------------------------------- */
        /* --- STAGE : 找到上行数据中需要的属性的值 ---------------------- */
        //TODO: 解决多数据包同时上行情况 (重复数据包接收是因为距离太近)
        //https://forum.rakwireless.com/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/3980/3?u=haowong
        //https://forum.chirpstack.io/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/10886/2?u=shirou_emiya

        Rxpk rxpk_array[GW];

        openFile();

        float SNRArray[GW];//存放所有SNR值

        for (int loopcount = 0; loopcount <= GW - 1; loopcount++) {
            rxpk_array[loopcount].setTime(buffer_array[loopcount].uint, buffer_array->buff_index);
            rxpk_array[loopcount].setStat(buffer_array[loopcount].uint, buffer_array->buff_index);
            rxpk_array[loopcount].setCrc_get(buffer_array[loopcount].uint, buffer_array->buff_index);
            rxpk_array[loopcount].setStr(buffer_array[loopcount].uint, buffer_array->buff_index);
            rxpk_array[loopcount].setRssi(buffer_array[loopcount].uint, buffer_array->buff_index);

            rxpk_array[loopcount].setSNR(buffer_array[loopcount].uint, buffer_array->buff_index);
            SNRArray[loopcount] = rxpk_array[loopcount].snr;

            rxpk_array[loopcount].setPayloadSize(buffer_array[loopcount].uint, buffer_array->buff_index);

#if DEBUG
            printf("\nrxpk%d.stat: %d\n", loopcount + 1, rxpk_array[loopcount].stat);
            printf("rxpk%d.crc_get: %d\n", loopcount + 1, rxpk_array[loopcount].crc_get);
            printf("rxpk%d.str: %s\n", loopcount + 1, rxpk_array[loopcount].str);
            printf("rxpk%d.rssi: %d\n", loopcount + 1, rxpk_array[loopcount].rssi);
            printf("rxpk%d.snr: %f\n", loopcount + 1, rxpk_array[loopcount].snr);
            printf("rxpk%d.time: %s\n", loopcount + 1, rxpk_array[loopcount].time);
            printf("rxpk%d.fcnt: %d\n", loopcount + 1, rxpk_array[0].fcnt);
            printf("rxpk%d.PayloadSize: %d\n\n", loopcount + 1, rxpk_array[0].PayloadSize);
#endif

            rxpk_array[loopcount].crc = new char[BUF_SIZE];
            memset(rxpk_array[loopcount].crc, 0, BUF_SIZE * sizeof(char));

            sprintf(rxpk_array[loopcount].crc, "0x%04X", rxpk_array[loopcount].crc_get);
            //logCRC(rxpk_array[loopcount].crc);

#if DEBUG
            printf("Processed CRC%d: %s\n", loopcount + 1, rxpk_array[loopcount].crc);
#endif
            char *ptr;
            rxpk_array[loopcount].crc_hex = strtoul(rxpk_array[loopcount].crc, &ptr, 16);
#if DEBUG
            printf("Processed CRC%d: 0x%04X\n", loopcount + 1, rxpk_array[loopcount].crc_hex);
#endif
        }

        /* -------------------------------------------------------------------------- */
        /* --- STAGE : Decoding ---------------------- */

        for (int loopcount = 0; loopcount <= GW - 1; loopcount++) {
            memset(buffer_array[loopcount].payload, 0, BUF_SIZE * sizeof(uint8_t));

            buffer_array[loopcount].setSize(rxpk_array[loopcount].str);//与net_downlink相似，都是接收到data，故都用b64_to_bin
            //logPHYPayload(buffer_array[loopcount].payload, buffer_array[0].size);
            logData(rxpk_array[loopcount].str);

#if DEGUG
            cout << "copy" << loopcount + 1 << " of data: " << rxpk_array[loopcount].str << endl;
#endif
            rxpk_array[loopcount].mote_addr = buffer_array[loopcount].payload[1];
            rxpk_array[loopcount].mote_addr |= buffer_array[loopcount].payload[2] << 8;
            rxpk_array[loopcount].mote_addr |= buffer_array[loopcount].payload[3] << 16;
            rxpk_array[loopcount].mote_addr |= buffer_array[loopcount].payload[4] << 24;
            /* FHDR - FCnt */
            rxpk_array[loopcount].mote_fcnt = buffer_array[loopcount].payload[6];
            rxpk_array[loopcount].mote_fcnt |= buffer_array[loopcount].payload[7];
            //logFcnt(rxpk_array[loopcount].mote_fcnt);

#if DEBUG
            printf("INFO: Received pkt%d from mote: %08X (fcnt=%u)\n", loopcount + 1, rxpk_array[loopcount].mote_addr, rxpk_array[loopcount].mote_fcnt);
#endif
        }

        uint8_t payload_array[GW][BUF_SIZE];//所有payload的集合
        for (int row = 0; row <= GW - 1; row++) {
            memset(payload_array[row], 0, BUF_SIZE * sizeof(char));

            for (int column = 0; column <= BUF_SIZE - 1; column++) {
                payload_array[row][column] = buffer_array[row].payload[column];
            }
        }

        /* -------------------------------------------------------------------------- */
        /* --- STAGE : 当全部上行数据都错且crc值相同时进行纠错 ---------------------- */

        if (compareStat(rxpk_array, GW)) {

            CRCErrorNumBefore++;

            if (compareDevAddr(rxpk_array, GW)) {

                if (compareCRC(rxpk_array, GW)) {

                    openErrorFile();

                    printf("/* ----------------------Error correction begins--------------------------------- */\n");

                    uint16_t size;
                    if (compareSize(buffer_array, GW)) {
                        size = buffer_array[0].size;
                    } else {
                        printf("Error: Not all copies has the same length! This program will be shut down!\n");
                        printf("/* ----------------------Error correction ends--------------------------------- */\n\n");
                        CRCErrorNumAfter++;
                        continue;
                    }

                    int Hamming_weight_now;

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : uint8_t转char ---------------------- *///https://bbs.csdn.net/topics/390141308

                    for (int loopcount = 0; loopcount <= GW - 1; loopcount++) {
                        buffer_array[loopcount].Hexstring = new char[BUF_SIZE];
                        memset(buffer_array[loopcount].Hexstring, 0, BUF_SIZE * sizeof(char));

                        buffer_array[loopcount].setHexstring();
                    }

                    for (int loopcount = 0; loopcount <= GW - 1; loopcount++) {
                        printf("PHY Payload%d get: %s\n", loopcount + 1, buffer_array[loopcount].Hexstring);
                    }

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 十六进制字符串转二进制字符串 ---------------------- *///https://blog.csdn.net/weixin_30279751/article/details/95437814

                    char BinaryArray[GW][BUF_SIZE];//所有二进制字符串的集合

                    for (int loopcount = 0; loopcount <= GW - 1; loopcount++) {
                        memset(BinaryArray[loopcount], 0, BUF_SIZE * sizeof(char));

                        buffer_array[loopcount].Binarystring = new char[BUF_SIZE];
                        memset(buffer_array[loopcount].Binarystring, 0, BUF_SIZE * sizeof(char));

                        buffer_array[loopcount].setBinarystring();
                        strcpy(BinaryArray[loopcount], buffer_array[loopcount].Binarystring);

                        delete[] buffer_array[loopcount].Hexstring;
                    }

                    for (int loopcount = 0; loopcount <= GW - 1; loopcount++) {
                        delete[] rxpk_array[loopcount].str;
                    }

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : GetCandidate ---------------------- */
                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : CRC ---------------------- */


                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 纠错 ---------------------- */

                    char *mch = new char[BUF_SIZE];
                    memset(mch, 0, BUF_SIZE * sizeof(char));

                    int index = compareSNR(rxpk_array, GW);//Selection Combining (SC)
#if DEBUG
                    printf("Chosen candidate index: %d\n", index);
#endif

                    char *fakeresult = new char[BUF_SIZE];//每次candidate与mch异或的中间产值
                    memset(fakeresult, 0, BUF_SIZE * sizeof(char));

                    char realresult[Concurrent][BUF_SIZE];
                    for (int loopcount = 0; loopcount <= Concurrent - 1; loopcount++) {
                        memset(realresult[loopcount], 0, BUF_SIZE * sizeof(char));
                    }

                    int total_number = 0;//一共运行的次数
                    int pass_crc = 0;    //符合CRC校验的次数

                    struct timespec startTime;
                    clock_gettime(CLOCK_MONOTONIC, &startTime);

                    switch (StageOption) {
                        case 0: {
                            /* -------------------------------------------------------------------------- */
                            /* --- STAGE : No Error Correction ---------------------- */
                            if (strlen(*realresult) == 0) {
                                printf("Choose no error correction method!\n");
                            }
                            break;
                        }
                        case 1: {
                            if (strlen(*realresult) == 0) {

                                /* -------------------------------------------------------------------------- */
                                /* --- STAGE : EPC ---------------------- */

                                printf("%s\n", "EPC start!");

                                buffer.Binarystring = new char[BUF_SIZE];//Merged error mask / Ambiguity vectors
                                memset(buffer.Binarystring, 0, BUF_SIZE * sizeof(char));

                                Hamming_weight_now = 0;
                                getMultipleNe(payload_array, GW, size, Hamming_weight_now);//Calculate Hamming weight

                                if (Hamming_weight_now > Hamming_weight_max) {

                                    printf("%s: %d\n", "Hamming weight is larger than the max number", Hamming_weight_max);
                                    printf("This program will be shut down!\n");
                                    printf("/* ----------------------Error correction ends--------------------------------- */\n\n");
                                    CRCErrorNumAfter++;
                                    continue;
                                }

                                printf("Hamming_weight_now: %d\n", Hamming_weight_now);

                                buffer.setMultipleBinarystring(BinaryArray, GW);

                                memset(mch, 0, BUF_SIZE * sizeof(char));
                                strcpy(mch, buffer_array[index].Binarystring);
#if DEBUG
                                printf("MCH: %s\n", mch);
                                printf("Chosen copy: %s\n", rxpk_array[index].str);
#endif

                                memset(fakeresult, 0, BUF_SIZE * sizeof(char));

                                for (int loopcount = 0; loopcount <= Concurrent - 1; loopcount++) {
                                    memset(realresult[loopcount], 0, BUF_SIZE * sizeof(char));
                                }
                                total_number = 0;//一共运行的次数
                                pass_crc = 0;    //符合CRC校验的次数

                                correct(buffer.Binarystring, mch, Hamming_weight_now, rxpk_array[0].crc_get, fakeresult, realresult, size, pass_crc, total_number, startTime, rxpk_array[index].mote_fcnt, rxpk_array[index].mote_addr);

                                delete[] buffer.Binarystring;

                                if (strlen(*realresult) == 0) {
                                    printf("%s\n", "Error can not be fixed with EPC!");
                                }
                            }
                        }
                        case 2: {

                            if (strlen(*realresult) == 0) {
                                printf("%s\n", "APC start!");
                                //CRC未出错的话一定出现了hidden error

                                /* -------------------------------------------------------------------------- */
                                /* --- STAGE : APC ---------------------- */

                                buffer.Binarystring2 = new char[BUF_SIZE];//Merged error mask / Ambiguity vectors / Va
                                memset(buffer.Binarystring2, 0, BUF_SIZE * sizeof(char));
                                buffer.Binarystring3 = new char[BUF_SIZE];//APC candidate
                                memset(buffer.Binarystring3, 0, BUF_SIZE * sizeof(char));

                                Hamming_weight_now = 0;

                                LeastReliableMask(BinaryArray, GW, buffer.Binarystring2, Hamming_weight_now);//calculate Hamming weight
                                majorityVoting(BinaryArray, GW, buffer.Binarystring3);

                                if (Hamming_weight_now > Hamming_weight_max) {

                                    printf("%s: %d\n", "Hamming weight is larger than the max number", Hamming_weight_max);
                                    printf("This program will be shut down!\n");
                                    printf("/* ----------------------Error correction ends--------------------------------- */\n\n");
                                    CRCErrorNumAfter++;
                                    continue;
                                }

                                if (FakeOption) {

                                } else {
                                    printf("Hamming_weight_now: %d\n", Hamming_weight_now);
                                }

                                memset(mch, 0, BUF_SIZE * sizeof(char));
                                strcpy(mch, buffer.Binarystring3);

#if DEBUG
                                printf("MCH: %s\n", mch);
                                printf("Chosen copy: %s\n", rxpk_array[index].str);
#endif

                                memset(fakeresult, 0, BUF_SIZE * sizeof(char));
                                for (int loopcount = 0; loopcount <= Concurrent - 1; loopcount++) {
                                    memset(realresult[loopcount], 0, BUF_SIZE * sizeof(char));
                                }
                                total_number = 0;//一共运行的次数
                                pass_crc = 0;    //符合CRC校验的次数

                                validateCRC(rxpk_array[0].crc_get, buffer.Binarystring3, realresult[0], size, pass_crc, rxpk_array[index].mote_fcnt, rxpk_array[index].mote_addr);

                                if (strlen(*realresult) == 0) {
                                    printf("%s\n", "Error can not be fixed! APC continues!");

                                    correct(buffer.Binarystring2, mch, Hamming_weight_now, rxpk_array[0].crc_get, fakeresult, realresult, size, pass_crc, total_number, startTime, rxpk_array[index].mote_fcnt, rxpk_array[index].mote_addr);

                                    delete[] buffer.Binarystring2;
                                    delete[] buffer.Binarystring3;

                                    if (strlen(*realresult) == 0) {
                                        printf("%s\n", "Error can not be fixed with APC!");
                                    }
                                }
                            }
                        }
                        case 3: {
                            if (strlen(*realresult) == 0) {
                                printf("%s\n", "Soft decoding begins!");
                                //CRC未出错的话一定出现了hidden error

                                /* -------------------------------------------------------------------------- */
                                /* --- STAGE : Soft decoding ---------------------- */

                                buffer.Binarystring4 = new char[BUF_SIZE];//Soft decoding candidate
                                memset(buffer.Binarystring4, 0, BUF_SIZE * sizeof(char));

                                for (int loopcount = 0; loopcount <= Concurrent - 1; loopcount++) {
                                    memset(realresult[loopcount], 0, BUF_SIZE * sizeof(char));
                                }
                                pass_crc = 0;//符合CRC校验的次数

                                softDecoding(BinaryArray, GW, buffer.Binarystring4, SNRArray);

                                validateCRC(rxpk_array[0].crc_get, buffer.Binarystring4, realresult[0], size, pass_crc, rxpk_array[index].mote_fcnt, rxpk_array[index].mote_addr);

                                delete[] buffer.Binarystring4;

                                if (strlen(*realresult) == 0) {
                                    printf("%s\n", "Error can not be fixed with soft decision!");
                                }
                            }
                            break;//防止执行到default分支
                        }
                        default: {
                            printf("StageOption is illegal! This program will be shut down!\n");
                            printf("/* ----------------------Error correction ends--------------------------------- */\n\n");
                            continue;
                        }
                    }


                    struct timespec endTime;
                    clock_gettime(CLOCK_MONOTONIC, &endTime);

                    if (FakeOption) {

                    } else {
                        printf("INFO: [up] Total time use in %i ms\n", (int) (1000 * difftimespec(endTime, startTime)));
                    }

                    for (int loopcount = 0; loopcount <= GW - 1; loopcount++) {
                        delete[] buffer_array[loopcount].Binarystring;
                        delete[] rxpk_array[loopcount].crc;
                    }

                    delete[] mch;
                    delete[] fakeresult;

                    for (int loopcount = 0; loopcount < Concurrent; loopcount++) {

                        if (strlen(realresult[loopcount]) == 0) {
                            continue;//防止通过crc校验的次数少于Concurrent(此时若crc值未出现问题则必定有通过MIC校验的结果出现)
                        }

                        CorrectedFlag = true;

#if DEBUG
                        printf("RealresultBit: %s\n", realresult);
#endif

#if DEBUG
                        if (pass_crc > 1) {//需更改if(flag == 1)判断条件为flag==2及以上数字，否则永远不会出现假阳性

                            printf("%s\n", "Falsepositive happens");
                        }
#endif

                        /* -------------------------------------------------------------------------- */
                        /* --- STAGE : 二进制字符串转十六进制字符串 ---------------------- */

                        buffer.Hexstring = new char[BUF_SIZE];//char类型的PHYPayload
                        memset(buffer.Hexstring, 0, BUF_SIZE * sizeof(char));

                        buffer.setHexstring(realresult[loopcount]);
#if DEBUG
                        printf("RealresultHex: %s\n", buffer.Hexstring);
#endif

                        /* -------------------------------------------------------------------------- */
                        /* --- STAGE : Encoding ---------------------- */

                        buffer.Hexstring_uint8 = new uint8_t[BUF_SIZE];
                        memset(buffer.Hexstring_uint8, 0, BUF_SIZE * sizeof(uint8_t));

                        buffer.setHexstring_uint();
                        printf("Corrected PHY Payload%d: ", loopcount + 1);
                        for (int count = 0; count < size; count++) {
                            printf("%02X", buffer.Hexstring_uint8[count]);
                        }
                        printf("\n");

                        delete[] buffer.Hexstring;


                        uint8_t *data_up_uint8 = new uint8_t[BUF_SIZE];
                        memset(data_up_uint8, 0, BUF_SIZE * sizeof(uint8_t));


                        bin_to_b64(buffer.Hexstring_uint8, size, (char *) (data_up_uint8), 341);
                        delete[] buffer.Hexstring_uint8;

                        char *data_up = new char[BUF_SIZE];//char类型的PHYPayload，即"data"里的字符串值
                        memset(data_up, 0, BUF_SIZE * sizeof(char));
                        strcpy(data_up, (char *) (data_up_uint8));
                        printf("Corrected data%d: %s\n", loopcount + 1, data_up);
                        delete[] data_up_uint8;

#if DEBUG
                        uint16_t payload_crc16_calc;
                        payload_crc16_calc = sx1302_lora_payload_crc(buffer.Hexstring_uint8, size);
                        printf("FixedPayload CRC (0x%04X)\n", payload_crc16_calc);
#endif

                        /* -------------------------------------------------------------------------- */
                        /* --- STAGE : 修改Upstream JSON data structure ---------------------- */
                        //TODO: 解决多数据包同时上行情况

                        buffer.inter = new char[BUF_SIZE];//将bufferi_inter赋值buffer_inter给以后续处理
                        memset(buffer.inter, 0, BUF_SIZE * sizeof(char));

                        buffer.inter_uint_char = new char[BUF_SIZE];//需要发送的数据的char形式（此时前12-byte header有缺陷，第12 byte后为修改后的Upstream JSON data structure）
                        memset(buffer.inter_uint_char, 0, BUF_SIZE * sizeof(char));

                        memset(buffer.send_first_part_char, 0, BUF_SIZE * sizeof(char));

                        memset(buffer.send_last_part_char, 0, BUF_SIZE * sizeof(char));

                        buffer.send = new uint8_t[BUF_SIZE];//需要发送的数据 (原始uint8形式)
                        memset(buffer.send, 0, BUF_SIZE * sizeof(uint8_t));

                        /* -------------------------------------------------------------------------- */
                        /* --- STAGE : 将Upstream JSON data structure的"data" field里面的数据使用修改后的data_up覆盖 ---------------------- */

                        strncpy(buffer_array[index].inter + FindFirstSubchar(buffer_array[index].inter, "data") + 6, data_up, strlen(data_up));//https://blog.csdn.net/zmhawk/article/details/44600075

#if DEBUG

                        //原生Json库
                        JSON_Value *root_val = NULL;
                        JSON_Object *first_obj = NULL;
                        JSON_Array *rxpk_array = NULL;

                        root_val = json_parse_string_with_comments((const char *) (buffer_array[index].uint + buffer_array->buff_index));
                        rxpk_array = json_object_get_array(json_value_get_object(root_val), "rxpk");
                        first_obj = json_array_get_object(rxpk_array, 0);
                        json_object_set_string(first_obj, "data", data_up);
                        buffer_array[index].inter = json_serialize_to_string(root_val);
                        puts(buffer_array[index].inter);

                        //CJson库 (https://github.com/DaveGamble/cJSON/issues/582)
                        cJSON *json = NULL;
                        cJSON *arrayItem = NULL;
                        cJSON *object = NULL;
                        cJSON *item = NULL;

                        json = cJSON_Parse((const char *) (buffer_array[index].uint + buffer_array->buff_index));
                        arrayItem = cJSON_GetObjectItem(json, "rxpk");
                        object = cJSON_GetArrayItem(arrayItem, 0);
                        item = cJSON_GetObjectItem(object, "data");
                        printf("data: %s\n", item->valuestring);
                        cJSON_SetValuestring(item, data_up);
                        buffer_array[index].inter = cJSON_Print(json);
                        puts(buffer_array[index].inter);

                        //两个库都无法做到这一点，只能手动写函数

#endif

                        /* -------------------------------------------------------------------------- */
                        /* --- STAGE : 更改stat从-1到1 ---------------------- */

                        buffer_array[index].inter_backup = new char[BUF_SIZE];
                        memset(buffer_array[index].inter_backup, 0, sizeof(char));
                        strcpy(buffer_array[index].inter_backup, buffer_array[index].inter);
                        buffer_array[index].index_backup = buffer_array[index].index;

                        deleteChar(buffer_array[index].inter_backup, FindFirstSubchar(buffer_array[index].inter_backup, "stat") + 5);
                        buffer_array[index].index_backup--;

                        /* -------------------------------------------------------------------------- */
                        /* --- STAGE : 构造出前12-byte header缺陷的buffer_inter_uint_char ---------------------- */

                        buffer.setInter(buffer_array[index].inter_backup);//将bufferi_inter赋值buffer_inter给以后续处理
                        buffer.setInter_Uint();
                        buffer.setInter_Uint_Char(buffer_array[index].index_backup);

                        /* -------------------------------------------------------------------------- */
                        /* --- STAGE : 将buff_i的前12-byte(必然不会被修改的header部分) 与buffer_inter_uint_char的第12 byte开始的部分(修改后的Upstream JSON data structure) 组合起来，转换为uint8_t的buffer_send ---------------------- */


                        buffer.setSend_First_Part_Char(buffer_array[index].data);
                        buffer.setSend_Last_Part_Char();
                        buffer.setSend();

                        printf("buffer%d.send: ", loopcount + 1);
                        for (int count = 0; count < buffer_array[index].index_backup; count++) {
                            printf("%02X", buffer.send[count]);
                        }
                        printf("\n");

                        printf("buffer%d.inter: %s\n", loopcount + 1, buffer.inter);

                        delete[] buffer_array[index].inter_backup;
                        delete[] data_up;
                        delete[] buffer.inter;
                        delete[] buffer.inter_uint_char;

                        /* -------------------------------------------------------------------------- */
                        /* --- STAGE : 发送---------------------- */

                        send(sock_up, (void *) buffer.send, buffer_array[index].index_backup, 0);
                    }

                    if (CorrectedFlag) {
                        NonCRCErrorNumAfter++;

                        throughoutData += size;

                    } else {
                        CRCErrorNumAfter++;
                    }

                    CorrectedFlag = false;//重新初始化Flag

                    PERAfter = CRCErrorNumAfter / (CRCErrorNumAfter + NonCRCErrorNumAfter);
                    PDRAfter = 1 - PERAfter;
                    printf("Packet error rate After: %f\n", PERAfter);
                    printf("Packet delivery rate After: %f\n", PDRAfter);
                    logPDRA(PDRAfter);

                    struct timespec ProEndTime;
                    clock_gettime(CLOCK_REALTIME, &ProEndTime);
                    struct tm t;
                    char date_time[BUF_SIZE];
                    strftime(date_time, sizeof(date_time), "%Y-%m-%dT%XZ",
                             localtime_r(&ProEndTime.tv_sec, &t));
                    logTimestamp(date_time);

                    printf("INFO: [up] Program total time use in %i ms\n", (int) (1000 * difftimespec(ProEndTime, ProStartTime)));
                    logTime((int) (1000 * difftimespec(ProEndTime, ProStartTime)));

                    cout << "Program throughoutData: " << throughoutData << " Bytes" << endl;
                    //logThroughoutData(throughoutData);

                    throughout = 1000 * double((throughoutData * 8 / 1000) / (int) (1000 * difftimespec(ProEndTime, ProStartTime)));
                    cout << "Program throughout: " << throughout << " kbps" << endl;
                    logThroughout(throughout);
                    logHammingWeight(Hamming_weight_now);

                    printf("/* ----------------------Error correction ends--------------------------------- */\n\n");

                    logLine();
                    logErrorLine();

                } else {

                    printf("/* ----------------------Special case begins--------------------------------- */\n");

                    CRCErrorNumAfter++;
                    PERAfter = CRCErrorNumAfter / (CRCErrorNumAfter + NonCRCErrorNumAfter);
                    PDRAfter = 1 - PERAfter;
                    printf("Packet error rate After: %f\n", PERAfter);
                    printf("Packet delivery rate After: %f\n", PDRAfter);
                    logPDRA(PDRAfter);

                    printf("Not all packets have the same FCS, no operation will be taken\n");

#if DEBUG
                    for (int loopcount = 0; loopcount <= GW - 1; loopcount++) {
                        cout << "buffer_send" << loopcount + 1 << ": ";
                        for (int count = 0; count < buffer_array[loopcount].index; count++) {
                            printf("%02X", buffer_array[loopcount].inter_uint[count]);
                        }
                        printf("\n\n");
                    }
#endif

                    struct timespec ProEndTime;
                    clock_gettime(CLOCK_REALTIME, &ProEndTime);
                    struct tm t;
                    char date_time[BUF_SIZE];
                    strftime(date_time, sizeof(date_time), "%Y-%m-%dT%XZ",
                             localtime_r(&ProEndTime.tv_sec, &t));
                    logTimestamp(date_time);

                    printf("INFO: [up] Program total time use in %i ms\n", (int) (1000 * difftimespec(ProEndTime, ProStartTime)));
                    logTime((int) (1000 * difftimespec(ProEndTime, ProStartTime)));

                    cout << "Program throughoutData: " << throughoutData << " Bytes" << endl;
                    //logThroughoutData(throughoutData);

                    throughout = 1000 * double((throughoutData * 8 / 1000) / (int) (1000 * difftimespec(ProEndTime, ProStartTime)));
                    cout << "Program throughout: " << throughout << " kbps" << endl;
                    logThroughout(throughout);

                    printf("/* ----------------------Special case ends--------------------------------- */\n\n");

                    logLine();

                    continue;
                }

            } else {

                printf("/* ----------------------Special case begins--------------------------------- */\n");

                CRCErrorNumAfter++;
                PERAfter = CRCErrorNumAfter / (CRCErrorNumAfter + NonCRCErrorNumAfter);
                PDRAfter = 1 - PERAfter;
                printf("Packet error rate After: %f\n", PERAfter);
                printf("Packet delivery rate After: %f\n", PDRAfter);
                logPDRA(PDRAfter);

                printf("At least one packet has error=“get device-session error: object does not exist\"\n");

#if DEBUG
                for (int loopcount = 0; loopcount <= GW - 1; loopcount++) {
                    cout << "buffer_send" << loopcount + 1 << ": ";
                    for (int count = 0; count < buffer_array[loopcount].index; count++) {
                        printf("%02X", buffer_array[loopcount].inter_uint[count]);
                    }
                    printf("\n\n");
                }
#endif

                struct timespec ProEndTime;
                clock_gettime(CLOCK_REALTIME, &ProEndTime);
                struct tm t;
                char date_time[BUF_SIZE];
                strftime(date_time, sizeof(date_time), "%Y-%m-%dT%XZ",
                         localtime_r(&ProEndTime.tv_sec, &t));
                logTimestamp(date_time);

                printf("INFO: [up] Program total time use in %i ms\n", (int) (1000 * difftimespec(ProEndTime, ProStartTime)));
                logTime((int) (1000 * difftimespec(ProEndTime, ProStartTime)));

                cout << "Program throughoutData: " << throughoutData << " Bytes" << endl;
                //logThroughoutData(throughoutData);

                throughout = 1000 * double((throughoutData * 8 / 1000) / (int) (1000 * difftimespec(ProEndTime, ProStartTime)));
                cout << "Program throughout: " << throughout << " kbps" << endl;
                logThroughout(throughout);

                printf("/* ----------------------Special case ends--------------------------------- */\n\n");

                logLine();

                continue;
            }

        } else {

            printf("/* ----------------------Special case begins--------------------------------- */\n");

            NonCRCErrorNumBefore++;
            PERBefore = CRCErrorNumBefore / (CRCErrorNumBefore + NonCRCErrorNumBefore);
            PDRBefore = 1 - PERBefore;
            if (FakeOption) {//deprecated because hamming_weight_now also indicates the harm situation
            } else {
                printf("Packet error rate Before: %f\n", PERBefore);
                printf("Packet delivery rate Before: %f\n", PDRBefore);
            }

            NonCRCErrorNumAfter++;
            PERAfter = CRCErrorNumAfter / (CRCErrorNumAfter + NonCRCErrorNumAfter);
            PDRAfter = 1 - PERAfter;
            printf("Packet error rate After: %f\n", PERAfter);
            logPDRA(PDRAfter);

            printf("Packet delivery rate After: %f\n", PDRAfter);

            printf("At least one packet is crc correct, no operation will be taken\n");

#if DEBUG
            for (int loopcount = 0; loopcount <= GW - 1; loopcount++) {
                cout << "buffer_send" << loopcount + 1 << ": ";
                for (int count = 0; count < buffer_array[loopcount].index; count++) {
                    printf("%02X", buffer_array[loopcount].inter_uint[count]);
                }
                printf("\n\n");
            }
#endif


            /* -------------------------------------------------------------------------- */
            /* --- STAGE : 发送---------------------- */

            for (int loopcount = 0; loopcount <= GW - 1; loopcount++) {
#if DEBUG
                cout << "buffer" << loopcount + 1 << ".inter: " << buffer_array[loopcount].inter << endl;
#endif
                send(sock_up, (void *) buffer_array[loopcount].inter_uint, buffer_array[loopcount].index, 0);
            }

            throughoutData += rxpk_array[0].PayloadSize;

            struct timespec ProEndTime;
            clock_gettime(CLOCK_REALTIME, &ProEndTime);
            struct tm t;
            char date_time[BUF_SIZE];
            strftime(date_time, sizeof(date_time), "%Y-%m-%dT%XZ",
                     localtime_r(&ProEndTime.tv_sec, &t));
            logTimestamp(date_time);

            printf("INFO: [up] Program total time use in %i ms\n", (int) (1000 * difftimespec(ProEndTime, ProStartTime)));
            logTime((int) (1000 * difftimespec(ProEndTime, ProStartTime)));

            cout << "Program throughoutData: " << throughoutData << " Bytes" << endl;
            //logThroughoutData(throughoutData);

            throughout = 1000 * double((throughoutData * 8 / 1000) / (int) (1000 * difftimespec(ProEndTime, ProStartTime)));
            cout << "Program throughout: " << throughout << " kbps" << endl;
            logThroughout(throughout);

            printf("/* ----------------------Special case ends--------------------------------- */\n\n");

            logLine();
        }
    }


    return 0;
}