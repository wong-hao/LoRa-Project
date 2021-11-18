#include "header_1_1.h"
#include "header_1_2.h"
#include "header_1_3.h"

#include "header_2_1.h"
#include "header_2_2.h"

#include "parson.h"
#include "payload_crc.h"

#include "payload_diff.h"

#include "header_3.h"

#include "base64.h"
#include "cJSON.h"

extern int sock_up;

int main()

{
    printf("Algorithm parameters: \n");
    printf("{\n    BUF_SIZE: %d\n", BUF_SIZE);
    printf("    Concurrent: %d\n", Concurrent);
    printf("    MAXLATENCY: %f\n", MAXLATENCY);
    printf("    Hamming_weight_max: %d\n", Hamming_weight_max);
    printf("    StageOption: %d\n", StageOption);
    printf("    deprecatedOption: %d\n}\n", deprecatedOption);


    int buffer_num = 4;
    Buffer buffer_array[buffer_num];

    BufferSend buffer{};

    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
        buffer_array[loopcount].data = new char[BUF_SIZE];
        memset(buffer_array[loopcount].data, 0, BUF_SIZE * sizeof(char));
    }

    struct timespec ProStartTime;
    clock_gettime(CLOCK_REALTIME, &ProStartTime);
    struct tm t;
    char date_time[BUF_SIZE];
    strftime(date_time, sizeof(date_time), "%Y-%m-%d-%H-%M-%S",
             localtime_r(&ProStartTime.tv_sec, &t));
    initFile(date_time);

    double throughoutData = 0;//PHY Payload
    double throughout = 0;

    bool CorrectedFlag = false;//防止纠错不成功后仍使得NonCRCErrorNum错误增加
    double CRCErrorNum = 0;
    double NonCRCErrorNum = 0;
    double PER;//计算无论经过纠错或未经过，最终未通过CRC校验的全局PER
    double PDR;

    while (1) {

        /* -------------------------------------------------------------------------- */
        /* --- STAGE : Socket接收到数据---------------------- */

        char *get1 = "02BAAB000016C001FF10D3F67B227278706B223A5B7B226A766572223A312C22746D7374223A31333736353431362C2274696D65223A22323032312D31312D31375431313A34363A33392E3030303030303030305A222C22746D6D73223A313332313138343739393030302C226368616E223A362C2272666368223A312C2266726571223A3438372E3530303030302C226D6964223A20382C2273746174223A2D312C226D6F6475223A224C4F5241222C2264617472223A22534631304257313235222C22636F6472223A22342F35222C227273736973223A2D36352C226C736E72223A2D362E382C22666F6666223A313536342C22637263223A32303530352C2244657641646472223A31343630383931372C2266636E74223A302C2272737369223A2D35392C2273697A65223A32362C2264617461223A22514258713367414141414142466858794972375133586B6F616C44785978784C586D513D227D5D7D";
        char *get2 = "0251FF000016C001FF10D3F77B227278706B223A5B7B226A766572223A312C22746D7374223A31313835353730332C2274696D65223A22323032312D31312D31375431313A34363A33392E3030303030303030305A222C22746D6D73223A313332313138343739393030302C226368616E223A362C2272666368223A312C2266726571223A3438372E3530303030302C226D6964223A20382C2273746174223A2D312C226D6F6475223A224C4F5241222C2264617472223A22534631304257313235222C22636F6472223A22342F35222C227273736973223A2D38322C226C736E72223A2D332E382C22666F6666223A313533392C22637263223A32303530352C2244657641646472223A31343630383931372C2266636E74223A302C2272737369223A2D37372C2273697A65223A32362C2264617461223A22514258713367414141414142466866366F72625933586B6F616B4377536C784C626C633D227D5D7D";
        char *get3 = "024AEC000016C001FF10D42D7B227278706B223A5B7B226A766572223A312C22746D7374223A31303239393936332C2274696D65223A22323032312D31312D31375431313A34363A33392E3030303030303030305A222C22746D6D73223A313332313138343739393030302C226368616E223A362C2272666368223A312C2266726571223A3438372E3530303030302C226D6964223A20382C2273746174223A2D312C226D6F6475223A224C4F5241222C2264617472223A22534631304257313235222C22636F6472223A22342F35222C227273736973223A2D37342C226C736E72223A2D352E352C22666F6666223A313530312C22637263223A32303530352C2244657641646472223A31343630383931372C2266636E74223A302C2272737369223A2D37302C2273697A65223A32362C2264617461223A22514258713367414141414142466850796F724C5A76586B6F616C43533668784C586A513D227D5D7D";
        char *get4 = "024AEC000016C001FF10D4467B227278706B223A5B7B226A766572223A312C22746D7374223A383731353734382C2274696D65223A22323032312D31312D31375431313A34363A33392E3030303030303030305A222C22746D6D73223A313332313138343739393030302C226368616E223A362C2272666368223A312C2266726571223A3438372E3530303030302C226D6964223A20382C2273746174223A2D312C226D6F6475223A224C4F5241222C2264617472223A22534631304257313235222C22636F6472223A22342F35222C227273736973223A2D36322C226C736E72223A2D362E382C22666F6666223A313538302C22637263223A32303530352C2244657641646472223A31343630383931372C2266636E74223A302C2272737369223A2D35362C2273697A65223A32362C2264617461223A2251425871336741414141414246682F796F724C5176586B6F616C44795131784C5867513D227D5D7D";

        char *buff_up_char1 = new char[BUF_SIZE];
        memset(buff_up_char1, 0, BUF_SIZE * sizeof(char));
        char *buff_up_char2 = new char[BUF_SIZE];
        memset(buff_up_char2, 0, BUF_SIZE * sizeof(char));
        char *buff_up_char3 = new char[BUF_SIZE];
        memset(buff_up_char3, 0, BUF_SIZE * sizeof(char));
        char *buff_up_char4 = new char[BUF_SIZE];
        memset(buff_up_char4, 0, BUF_SIZE * sizeof(char));

        strcpy(buff_up_char1, get1);
        strcpy(buff_up_char2, get2);
        strcpy(buff_up_char3, get3);
        strcpy(buff_up_char4, get4);

        /* -------------------------------------------------------------------------- */
        /* --- STAGE : Fake---------------------- */

#if DEBUG
        replaceData1(buff_up_char1);
        replaceData2(buff_up_char2);
        replaceData3(buff_up_char3);
        replaceData4(buff_up_char4);

        replaceStat(buff_up_char1);
        replaceStat(buff_up_char2);
        replaceStat(buff_up_char3);
        replaceStat(buff_up_char4);
#endif

        /* -------------------------------------------------------------------------- */
        /* --- STAGE : deal with the message---------------------- */

        buffer_array[0].setData(buff_up_char1);
        buffer_array[1].setData(buff_up_char2);
        buffer_array[2].setData(buff_up_char3);
        buffer_array[3].setData(buff_up_char4);

        for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
            buffer_array[loopcount].setIndex();
            buffer_array[loopcount].uint[BUF_SIZE] = {0};
            memset(buffer_array[loopcount].uint, 0, BUF_SIZE * sizeof(uint8_t));
            buffer_array[loopcount].setUint();
        }

        /* -------------------------------------------------------------------------- */
        /* --- STAGE : 对中间数据buffer_inter纠错---------------------- */

        for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
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

        Rxpk rxpk_array[buffer_num];


        for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
            rxpk_array[loopcount].setDevAddr_get(buffer_array[loopcount].uint, buffer_array->buff_index);
            rxpk_array[loopcount].setTime(buffer_array[loopcount].uint, buffer_array->buff_index);
            rxpk_array[loopcount].setStat(buffer_array[loopcount].uint, buffer_array->buff_index);
            rxpk_array[loopcount].setCrc_get(buffer_array[loopcount].uint, buffer_array->buff_index);
            rxpk_array[loopcount].setStr(buffer_array[loopcount].uint, buffer_array->buff_index);
            rxpk_array[loopcount].setRssi(buffer_array[loopcount].uint, buffer_array->buff_index);
            rxpk_array[loopcount].setPayloadSize(buffer_array[loopcount].uint, buffer_array->buff_index);
        }

#if DEBUG
        printf("rxpk1.DevAddr_get: %d\n", rxpk_array[0].DevAddr_get);
        printf("rxpk1.stat: %d\n", rxpk_array[0].stat);
        printf("rxpk1.crc_get: %d\n", rxpk_array[0].crc_get);
        printf("rxpk1.str: %s\n", rxpk_array[0].str);
        printf("rxpk1.rssi: %d\n", rxpk_array[0].rssi);
        printf("rxpk1.time: %s\n", rxpk_array[0].time);
        printf("rxpk1.PayloadSize: %d\n", rxpk_array[0].PayloadSize);
#endif

        /* -------------------------------------------------------------------------- */
        /* --- STAGE : 当全部上行数据都错且crc值相同时进行纠错 ---------------------- */

        if (compareStat(rxpk_array, buffer_num)) {

            if (compareDevAddr(rxpk_array, buffer_num)) {

                if (compareCRC(rxpk_array, buffer_num)) {

                    printf("/* ----------------------Error correction begins--------------------------------- */\n");

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : Decoding ---------------------- */

                    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                        buffer_array[loopcount].payload[BUF_SIZE] = {0};
                        memset(buffer_array[loopcount].payload, 0, BUF_SIZE * sizeof(uint8_t));

                        buffer_array[loopcount].setSize(rxpk_array[loopcount].str);//与net_downlink相似，都是接收到data，故都用b64_to_bin
                        cout << "copy" << loopcount + 1 << " of data: " << rxpk_array[loopcount].str << endl;
                    }

                    uint16_t size;

                    if ((buffer_array[0].size == buffer_array[1].size) && (buffer_array[0].size == buffer_array[2].size) && (buffer_array[0].size == buffer_array[3].size)) {
                        size = buffer_array[0].size;
                    } else {
                        printf("Error: Not all copies has the same length! This program will be shut down!\n");
                        printf("/* ----------------------Error correction ends--------------------------------- */\n\n");
                        continue;
                    }

                    int Hamming_weight_now;

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : uint8_t转char ---------------------- *///https://bbs.csdn.net/topics/390141308

                    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                        buffer_array[loopcount].Hexstring = new char[BUF_SIZE];
                        memset(buffer_array[loopcount].Hexstring, 0, BUF_SIZE * sizeof(char));

                        buffer_array[loopcount].setHexstring();
                    }

                    openFile();
                    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                        printf("PHY Payload%d get: %s\n", loopcount + 1, buffer_array[loopcount].Hexstring);
                        logPHYPayload(buffer_array[loopcount].Hexstring);
                    }

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 十六进制字符串转二进制字符串 ---------------------- *///https://blog.csdn.net/weixin_30279751/article/details/95437814

                    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                        buffer_array[loopcount].Binarystring = new char[BUF_SIZE];
                        memset(buffer_array[loopcount].Binarystring, 0, BUF_SIZE * sizeof(char));

                        buffer_array[loopcount].setBinarystring();
                        delete[] buffer_array[loopcount].Hexstring;
                    }

                    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                        delete[] rxpk_array[loopcount].str;
                    }

                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : GetCandidate ---------------------- */
                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : CRC ---------------------- */

                    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                        buffer_array[loopcount].crc = new char[BUF_SIZE];
                        memset(buffer_array[loopcount].crc, 0, BUF_SIZE * sizeof(char));

                        sprintf(buffer_array[loopcount].crc, "0x%04X", rxpk_array[loopcount].crc_get);
#if DEBUG
                        printf("Processed CRC%d: %s\n", loopcount + 1, buffer_array[loopcount].crc);
#endif
                        logCRC(buffer_array[loopcount].crc);
                        sscanf(buffer_array[loopcount].crc, "%X", &buffer_array[loopcount].crc_int);
                    }

                    logLine();

#if DEBUG
                    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                        buffer_array[loopcount].DevAddr = new char[BUF_SIZE];
                        memset(buffer_array[loopcount].DevAddr, 0, BUF_SIZE * sizeof(char));

                        sprintf(buffer_array[loopcount].DevAddr, "0x%04X", rxpk_array[loopcount].DevAddr_get);
                        printf("Processed DevAddr%d: %s\n", loopcount + 1, buffer_array[loopcount].DevAddr);
                    }
#endif
                    /* -------------------------------------------------------------------------- */
                    /* --- STAGE : 纠错 ---------------------- */

                    char *mch = new char[BUF_SIZE];
                    memset(mch, 0, BUF_SIZE * sizeof(char));

                    int index = compareRSSI(rxpk_array, buffer_num);//Selection Combining (SC)

                    char *fakeresult = new char[BUF_SIZE];//每次candidate与mch异或的中间产值
                    memset(fakeresult, 0, BUF_SIZE * sizeof(char));

                    char realresult[Concurrent][BUF_SIZE];//符合CRC校验的fakeresult，但不保证能通过MIC校验
                    for (int loopcount = 0; loopcount <= Concurrent - 1; loopcount++) {
                        memset(realresult[loopcount], 0, BUF_SIZE * sizeof(char));
                    }

                    int total_number = 0;//一共运行的次数
                    int pass_crc = 0;    //符合CRC校验的次数

                    struct timespec startTime;
                    clock_gettime(CLOCK_REALTIME, &startTime);

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
                                getFourthNe(buffer_array[0].payload, buffer_array[1].payload, buffer_array[2].payload, buffer_array[3].payload, size, Hamming_weight_now);//Calculate Hamming weight

                                if (Hamming_weight_now > Hamming_weight_max) {

                                    printf("%s: %d\n", "Hamming weight is larger than the max number", Hamming_weight_max);
                                    printf("This program will be shut down!\n");
                                    printf("/* ----------------------Error correction ends--------------------------------- */\n\n");
                                    continue;
                                }

                                printf("Hamming_weight_now: %d\n", Hamming_weight_now);

                                buffer.setForthBinarystring(buffer_array[0].Binarystring, buffer_array[1].Binarystring, buffer_array[2].Binarystring, buffer_array[3].Binarystring);

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

                                if (compareCRC2(rxpk_array, buffer_num)) {
                                    if (Hamming_weight_now <= Hamming_weight_max / 2) {
                                        incremental_correct(buffer.Binarystring, mch, Hamming_weight_now, buffer_array[0].crc_int, fakeresult, realresult, size, pass_crc, total_number, startTime);
                                    } else {
                                        correct(buffer.Binarystring, mch, Hamming_weight_now, buffer_array[0].crc_int, fakeresult, realresult, size, pass_crc, total_number, startTime);
                                    }
                                } else if (compareCRC3(rxpk_array)) {
                                    if (Hamming_weight_now <= Hamming_weight_max / 2) {
                                        incremental_correct(buffer.Binarystring, mch, Hamming_weight_now, compareCRC3(rxpk_array), fakeresult, realresult, size, pass_crc, total_number, startTime);
                                    } else {
                                        correct(buffer.Binarystring, mch, Hamming_weight_now, compareCRC3(rxpk_array), fakeresult, realresult, size, pass_crc, total_number, startTime);
                                    }
                                } else {
                                    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                                        if (Hamming_weight_now <= Hamming_weight_max / 2) {
                                            incremental_correct(buffer.Binarystring, mch, Hamming_weight_now, buffer_array[loopcount].crc_int, fakeresult, realresult, size, pass_crc, total_number, startTime);
                                        } else {
                                            correct(buffer.Binarystring, mch, Hamming_weight_now, buffer_array[loopcount].crc_int, fakeresult, realresult, size, pass_crc, total_number, startTime);
                                        }
                                    }
                                }

                                delete[] buffer.Binarystring;

                                if (strlen(*realresult) == 0) {
                                    printf("%s\n", "Error can not be fixed with EPC!");
                                }
                            }
                            break;
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

                                LeastReliableMask(buffer_array[0].Binarystring, buffer_array[1].Binarystring, buffer_array[2].Binarystring, buffer_array[3].Binarystring, buffer.Binarystring2, Hamming_weight_now);//calculate Hamming weight
                                majorityVoting(buffer_array[0].Binarystring, buffer_array[1].Binarystring, buffer_array[2].Binarystring, buffer_array[3].Binarystring, buffer.Binarystring3);

                                if (Hamming_weight_now > Hamming_weight_max) {

                                    printf("%s: %d\n", "Hamming weight is larger than the max number", Hamming_weight_max);
                                    printf("This program will be shut down!\n");
                                    printf("/* ----------------------Error correction ends--------------------------------- */\n\n");
                                    continue;
                                }

                                printf("Hamming_weight_now: %d\n", Hamming_weight_now);

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

                                if (compareCRC2(rxpk_array, buffer_num)) {
                                    validateCRC(buffer_array[0].crc_int, buffer.Binarystring3, realresult[0], size, pass_crc);
                                } else if (compareCRC3(rxpk_array)) {
                                    validateCRC(compareCRC3(rxpk_array), buffer.Binarystring3, realresult[0], size, pass_crc);
                                } else {
                                    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                                        validateCRC(buffer_array[loopcount].crc_int, buffer.Binarystring3, realresult[0], size, pass_crc);
                                    }
                                }

                                if (strlen(*realresult) == 0) {
                                    printf("%s\n", "Error can not be fixed! APC continues!");

                                    if (compareCRC2(rxpk_array, buffer_num)) {
                                        if (Hamming_weight_now <= Hamming_weight_max / 2) {
                                            incremental_correct(buffer.Binarystring2, mch, Hamming_weight_now, buffer_array[0].crc_int, fakeresult, realresult, size, pass_crc, total_number, startTime);
                                        } else {
                                            correct(buffer.Binarystring2, mch, Hamming_weight_now, buffer_array[0].crc_int, fakeresult, realresult, size, pass_crc, total_number, startTime);
                                        }
                                    } else if (compareCRC3(rxpk_array)) {
                                        if (Hamming_weight_now <= Hamming_weight_max / 2) {
                                            incremental_correct(buffer.Binarystring2, mch, Hamming_weight_now, compareCRC3(rxpk_array), fakeresult, realresult, size, pass_crc, total_number, startTime);
                                        } else {
                                            correct(buffer.Binarystring2, mch, Hamming_weight_now, compareCRC3(rxpk_array), fakeresult, realresult, size, pass_crc, total_number, startTime);
                                        }
                                    } else {
                                        for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                                            if (Hamming_weight_now <= Hamming_weight_max / 2) {
                                                incremental_correct(buffer.Binarystring2, mch, Hamming_weight_now, buffer_array[loopcount].crc_int, fakeresult, realresult, size, pass_crc, total_number, startTime);
                                            } else {
                                                correct(buffer.Binarystring2, mch, Hamming_weight_now, buffer_array[loopcount].crc_int, fakeresult, realresult, size, pass_crc, total_number, startTime);
                                            }
                                        }
                                    }

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

                                softDecoding(buffer_array[0].Binarystring, buffer_array[1].Binarystring, buffer_array[2].Binarystring, buffer_array[3].Binarystring, buffer.Binarystring4, rxpk_array[0].rssi, rxpk_array[1].rssi, rxpk_array[2].rssi, rxpk_array[3].rssi);

                                if (compareCRC2(rxpk_array, buffer_num)) {
                                    validateCRC(buffer_array[0].crc_int, buffer.Binarystring4, realresult[0], size, pass_crc);
                                } else if (compareCRC3(rxpk_array)) {
                                    validateCRC(compareCRC3(rxpk_array), buffer.Binarystring4, realresult[0], size, pass_crc);
                                } else {
                                    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                                        validateCRC(buffer_array[loopcount].crc_int, buffer.Binarystring4, realresult[0], size, pass_crc);
                                    }
                                }

                                delete[] buffer.Binarystring4;

                                if (strlen(*realresult) == 0) {
                                    printf("%s\n", "Error can not be fixed with soft decision!");
                                }
                            }
                            break;//防止执行到default分支
                        }
                        default: {
                            printf("StageOption is illegal! This program will be shut down!\n");
                            continue;
                        }
                    }


                    struct timespec endTime;
                    clock_gettime(CLOCK_REALTIME, &endTime);

                    struct timespec interv;
                    diff(&startTime, &endTime, &interv);
                    cout << "Total timeuse: " << double(interv.tv_sec * NANOSECOND + interv.tv_nsec) / NANOSECOND << "s" << endl;

                    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                        delete[] buffer_array[loopcount].Binarystring;
                        delete[] buffer_array[loopcount].crc;
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
                        for (int loopcount = 0; loopcount < size; loopcount++) {
                            printf("%02X", buffer.Hexstring_uint8[loopcount]);
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

                        buffer.send_first_part_char[BUF_SIZE] = {0};//12-byte header
                        memset(buffer.send_first_part_char, 0, BUF_SIZE * sizeof(char));

                        buffer.send_last_part_char[BUF_SIZE] = {0};//修改后的Upstream JSON data structure
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
                        NonCRCErrorNum++;

                        throughoutData += size;

                        struct timespec ProEndTime;
                        clock_gettime(CLOCK_REALTIME, &ProEndTime);

                        struct timespec ProInterv;
                        diff(&ProStartTime, &ProEndTime, &ProInterv);
                        cout << "Program total timeuse: " << double(ProInterv.tv_sec * NANOSECOND + ProInterv.tv_nsec) / NANOSECOND << "s" << endl;

                        cout << "Program throughoutData: " << throughoutData << "Byte" << endl;
                        throughout = (throughoutData * 8 / 1000) / (double(ProInterv.tv_sec * NANOSECOND + ProInterv.tv_nsec) / NANOSECOND);
                        cout << "Program throughout: " << throughout << "kbps" << endl;
                    } else {
                        CRCErrorNum++;
                    }

                    CorrectedFlag = false;//重新初始化Flag

                    PER = CRCErrorNum / (CRCErrorNum + NonCRCErrorNum);
                    PDR = 1 - PER;
                    printf("Packet error rate: %f\n", PER);
                    printf("Packet delivery rate: %f\n", PDR);

                    printf("/* ----------------------Error correction ends--------------------------------- */\n\n");


                } else {

                    printf("/* ----------------------Special case begins--------------------------------- */\n");

                    CRCErrorNum++;
                    PER = CRCErrorNum / (CRCErrorNum + NonCRCErrorNum);
                    PDR = 1 - PER;
                    printf("Packet error rate: %f\n", PER);
                    printf("Packet delivery rate: %f\n", PDR);

                    printf("Not all packets have the same FCS, no operation will be taken\n");

#if DEBUG
                    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                        cout << "buffer_send" << loopcount + 1 << ": ";
                        for (int count = 0; count < buffer_array[loopcount].index; count++) {
                            printf("%02X", buffer_array[loopcount].inter_uint[count]);
                        }
                        printf("\n\n");
                    }
#endif

                    printf("/* ----------------------Special case ends--------------------------------- */\n\n");

                    continue;
                }

            } else {

                printf("/* ----------------------Special case begins--------------------------------- */\n");

                CRCErrorNum++;
                PER = CRCErrorNum / (CRCErrorNum + NonCRCErrorNum);
                PDR = 1 - PER;
                printf("Packet error rate: %f\n", PER);
                printf("Packet delivery rate: %f\n", PDR);

                printf("At least one packet has error=“get device-session error: object does not exist\"\n");

#if DEBUG
                for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                    cout << "buffer_send" << loopcount + 1 << ": ";
                    for (int count = 0; count < buffer_array[loopcount].index; count++) {
                        printf("%02X", buffer_array[loopcount].inter_uint[count]);
                    }
                    printf("\n\n");
                }
#endif

                printf("/* ----------------------Special case ends--------------------------------- */\n\n");

                continue;
            }

        } else {

            printf("/* ----------------------Special case begins--------------------------------- */\n");

            NonCRCErrorNum++;
            PER = CRCErrorNum / (CRCErrorNum + NonCRCErrorNum);
            PDR = 1 - PER;
            printf("Packet error rate: %f\n", PER);
            printf("Packet delivery rate: %f\n", PDR);

            printf("At least one packet is crc correct, no operation will be taken\n");

#if DEBUG
            for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                cout << "buffer_send" << loopcount + 1 << ": ";
                for (int count = 0; count < buffer_array[loopcount].index; count++) {
                    printf("%02X", buffer_array[loopcount].inter_uint[count]);
                }
                printf("\n\n");
            }
#endif


            /* -------------------------------------------------------------------------- */
            /* --- STAGE : 发送---------------------- */

            for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
#if DEBUG
                cout << "buffer" << loopcount + 1 << ".inter: " << buffer_array[loopcount].inter << endl;
#endif
                send(sock_up, (void *) buffer_array[loopcount].inter_uint, buffer_array[loopcount].index, 0);
            }

            throughoutData += rxpk_array[0].PayloadSize;

            struct timespec ProEndTime;
            clock_gettime(CLOCK_REALTIME, &ProEndTime);

            struct timespec ProInterv;
            diff(&ProStartTime, &ProEndTime, &ProInterv);
            cout << "Program total timeuse: " << double(ProInterv.tv_sec * NANOSECOND + ProInterv.tv_nsec) / NANOSECOND << "s" << endl;

            cout << "Program throughoutData: " << throughoutData << "Byte" << endl;
            throughout = (throughoutData * 8 / 1000) / (double(ProInterv.tv_sec * NANOSECOND + ProInterv.tv_nsec) / NANOSECOND);
            cout << "Program throughout: " << throughout << "kbps" << endl;

            printf("/* ----------------------Special case ends--------------------------------- */\n\n");
        }
    }


    return 0;
}