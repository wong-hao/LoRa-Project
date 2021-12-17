//https://www.cnblogs.com/wuyepeng/p/9726771.html

#include "header_1_1.h"
#include "header_1_2.h"
#include "header_1_3.h"
#include "header_1_6.h"
#include "header_2_1.h"
#include "header_2_2.h"
#include "parson.h"
#include "payload_crc.h"

#include "payload_diff.h"

#include "header_3.h"

#include "base64.h"
#include "timelib.h"


int main() {


    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 建立发射socket ---------------------- */

    printf("Algorithm parameters: \n");
    printf("{\n    BUF_SIZE: %d\n", BUF_SIZE);
    printf("    MAXLATENCY: %f\n", MAXLATENCY);
    printf("    Hamming_weight_max: %d\n", Hamming_weight_max);
    printf("    StageOption: %d\n", StageOption);
    printf("    FakeOption: %d\n", FakeOption);

    if (MICOption) {
        printf("    MICOption: %d\n", MICOption);
    } else {
        printf("    MICOption: %d\n", MICOption);
        printf("    Concurrent: %d\n", Concurrent);
    }

    if (Concurrent == 1) {
        printf("    Concurrent: %d\n", Concurrent);
    } else {
        printf("    Concurrent: %d (Should be '1'), the program will be shut down\n", Concurrent);
        printf("    DeprecatedOption: %d\n}\n", DeprecatedOption);
        return 0;
    }
    printf("    DeprecatedOption: %d\n}\n", DeprecatedOption);
    printf("The error control server (port: %d) waits for connections and forward to Network server (address: %s, port: %s)!\n", ser_port, serv_addr, serv_port_up);

    int i = create_up_socket();
    if (i == -1) abort();

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 开始处理数据 ---------------------- */

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

    double CRCErrorNumBefore = 0;
    double NonCRCErrorNumBefore = 0;
    double PERBefore;
    double PDRBefore;

    bool CorrectedFlag = false; //防止纠错不成功后仍使得NonCRCErrorNumAfter错误增加
    double CRCErrorNumAfter = 0;//计算无论经过纠错或未经过，最终未通过CRC校验的次数
    double NonCRCErrorNumAfter = 0;
    double PERAfter;//计算无论经过纠错或未经过，最终未通过CRC校验的全局PER
    double PDRAfter;

    int ser_souck_fd;

    char input_message[BUF_SIZE] = {0};
    char *buff_up_char = new char[BUF_SIZE];

    struct sockaddr_in ser_addr;
    ser_addr.sin_family = AF_INET;//IPV4
    ser_addr.sin_port = htons(ser_port);
    ser_addr.sin_addr.s_addr = INADDR_ANY;//指定的是所有地址

    //creat socket
    if ((ser_souck_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("creat failure");
        return -1;
    }

    // 设置套接字选项避免地址使用错误 (https://www.cnblogs.com/argenbarbie/p/4118783.html)
    int on = 1;
    if ((setsockopt(ser_souck_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) {
        perror("setsockopt failed");

        return -1;
    }

    //bind soucket
    if (bind(ser_souck_fd, (const struct sockaddr *) &ser_addr, sizeof(ser_addr)) < 0) {
        perror("bind failure");
        return -1;
    }

    //listen
    if (listen(ser_souck_fd, backlog) < 0) {
        perror("listen failure");
        return -1;
    }


    //fd_set
    fd_set ser_fdset;
    int max_fd = 1;
    struct timeval mytime;
    //printf("wait for client connnect!\n");

    while (true) {
        mytime.tv_sec = 27;
        mytime.tv_usec = 0;

        FD_ZERO(&ser_fdset);

        //add standard input
        FD_SET(0, &ser_fdset);
        if (max_fd < 0) {
            max_fd = 0;
        }

        //add serverce
        FD_SET(ser_souck_fd, &ser_fdset);
        if (max_fd < ser_souck_fd) {
            max_fd = ser_souck_fd;
        }

        //add client
        for (int i = 0; i < CLI_NUM; i++)//用数组定义多个客户端fd
        {
            if (client_fds[i] != 0) {
                FD_SET(client_fds[i], &ser_fdset);
                if (max_fd < client_fds[i]) {
                    max_fd = client_fds[i];
                }
            }
        }

        //select多路复用
        int ret = select(max_fd + 1, &ser_fdset, NULL, NULL, &mytime);

        if (ret < 0) {
            perror("select failure\n");
            continue;
        }

        else if (ret == 0) {
            //printf("time out!");
            continue;
        }

        else {
            if (FD_ISSET(0, &ser_fdset))//标准输入是否存在于ser_fdset集合中（也就是说，检测到输入时，做如下事情）
            {
                printf("send message to");
                memset(input_message, 0, BUF_SIZE * sizeof(char));
                fgets(input_message, BUF_SIZE, stdin);

                for (int i = 0; i < CLI_NUM; i++) {
                    if (client_fds[i] != 0) {
                        printf("client_fds[%d]=%d\n", i, client_fds[i]);
                        send(client_fds[i], input_message, BUF_SIZE, 0);
                    }
                }
            }

            if (FD_ISSET(ser_souck_fd, &ser_fdset)) {
                struct sockaddr_in client_address;
                socklen_t address_len;
                int client_sock_fd = accept(ser_souck_fd, (struct sockaddr *) &client_address, &address_len);
                if (client_sock_fd > 0) {
                    int flags = -1;
                    //一个客户端到来分配一个fd，CLI_NUM=3，则最多只能有三个客户端，超过4以后跳出for循环，flags重新被赋值为-1
                    for (int i = 0; i < CLI_NUM; i++) {
                        if (client_fds[i] == 0) {
                            flags = i;
                            client_fds[i] = client_sock_fd;
                            break;
                        }
                    }


                    if (flags >= 0) {
                        //printf("new user client[%d] add sucessfully!\n", flags);

                    }

                    else//flags=-1
                    {
                        char full_message[] = "the client is full!can't join!\n";
                        memset(input_message, 0, BUF_SIZE * sizeof(char));
                        strncpy(input_message, full_message, 100);
                        send(client_sock_fd, input_message, BUF_SIZE, 0);
                    }
                }
            }
        }

        //deal with the message

        for (int i = 0; i < CLI_NUM; i++) {
            if (client_fds[i] != 0) {
                if (FD_ISSET(client_fds[i], &ser_fdset)) {
                    memset(buff_up_char, 0, BUF_SIZE * sizeof(char));
                    int byte_num = read(client_fds[i], buff_up_char, BUF_SIZE);
                    if (byte_num > 0) {
                        //printf("message form client[%d]:%s\n", i, buff_up_char);

                        char *Gateway_unique_identifier = new char[MAC_address_length];
                        memset(Gateway_unique_identifier, 0, MAC_address_length * sizeof(char));
                        Gateway_unique_identifier[MAC_address_length] = '\0';
                        strncpy(Gateway_unique_identifier, buff_up_char + MAC_address_length / 2, MAC_address_length);
                        if (strcmp(Gateway_unique_identifier, MAC_address1) == 0) {
#if FakeOption
                            replaceData1(buff_up_char);
                            replaceStat(buff_up_char);
#endif
                            buffer_array[0].setData(buff_up_char);
                        } else if (strcmp(Gateway_unique_identifier, MAC_address2) == 0) {
#if FakeOption
                            replaceData2(buff_up_char);
                            replaceStat(buff_up_char);
#endif
                            buffer_array[1].setData(buff_up_char);
                        } else if (strcmp(Gateway_unique_identifier, MAC_address3) == 0) {
#if FakeOption
                            replaceData3(buff_up_char);
                            replaceStat(buff_up_char);
#endif
                            buffer_array[2].setData(buff_up_char);
                        } else if (strcmp(Gateway_unique_identifier, MAC_address4) == 0) {
#if FakeOption
                            replaceData4(buff_up_char);
                            replaceStat(buff_up_char);
#endif
                            buffer_array[3].setData(buff_up_char);
                        }

#if DEBUG
                        //TODO: 检查socketexample初始时单个网关是否会导致多接收/以及这里的buffer是否，若是则需要换框架
                        for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                            cout << "buffer" << loopcount + 1 << ": " << buffer_array[loopcount].data << endl;
                        }

                        printf("\n");
#endif

                        for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                            buffer_array[loopcount].setIndex();
                            memset(buffer_array[loopcount].uint, 0, BUF_SIZE * sizeof(uint8_t));
                            buffer_array[loopcount].setUint();
                        }

                        /* -------------------------------------------------------------------------- */
                        /* --- STAGE : 对中间数据buffer_inter纠错---------------------- */
                        //TODO: false and true带来的多个包同时转发


                        for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                            buffer_array[loopcount].inter = new char[BUF_SIZE];
                            memset(buffer_array[loopcount].inter, 0, BUF_SIZE * sizeof(char));
                            buffer_array[loopcount].setInter();
#if DEBUG
                            cout << "buffer" << loopcount + 1 << ".inter: " << buffer_array[loopcount].inter << endl;
#endif
                            buffer_array[loopcount].inter_uint = new uint8_t[BUF_SIZE];
                            memset(buffer_array[loopcount].inter_uint, 0, BUF_SIZE * sizeof(uint8_t));
                            buffer_array[loopcount].setInter_Uint();
                        }

                        /* -------------------------------------------------------------------------- */
                        /* --- STAGE : select的异步处理---------------------- */

                        Rxpk rxpk_array[buffer_num];

                        for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                            rxpk_array[loopcount].setTime(buffer_array[loopcount].uint, buffer_array->buff_index);
                            rxpk_array[loopcount].setFcnt(buffer_array[loopcount].uint, buffer_array->buff_index);
                        }

                        switch (countED(buffer_array, buffer_num)) {
                            case 4: {
                                if (compareTime(rxpk_array, buffer_num) || compareFcnt(rxpk_array, buffer_num)) {
                                    printf("buffer_send1: ");
                                    for (int count = 0; count < buffer_array[0].index; count++) {
                                        printf("%02X", buffer_array[0].inter_uint[count]);
                                    }
                                    printf("\n");

                                    printf("buffer_send2: ");
                                    for (int count = 0; count < buffer_array[1].index; count++) {
                                        printf("%02X", buffer_array[1].inter_uint[count]);
                                    }
                                    printf("\n");

                                    printf("buffer_send3: ");
                                    for (int count = 0; count < buffer_array[2].index; count++) {
                                        printf("%02X", buffer_array[2].inter_uint[count]);
                                    }
                                    printf("\n");

                                    printf("buffer_send4: ");
                                    for (int count = 0; count < buffer_array[3].index; count++) {
                                        printf("%02X", buffer_array[3].inter_uint[count]);
                                    }
                                    printf("\n");

                                    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                                        cout << "buffer" << loopcount + 1 << ".inter: " << buffer_array[loopcount].inter << endl;
                                        //send(sock_up, (void*)buffer_array[loopcount].inter_uint, buffer_array[loopcount].index, 0);
                                    }

                                    printf("\n");

                                    /* -------------------------------------------------------------------------- */
                                    /* --- STAGE : 找到上行数据中需要的属性的值 ---------------------- */
                                    //TODO: 解决多数据包同时上行情况 (重复数据包接收是因为距离太近)
                                    //https://forum.rakwireless.com/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/3980/3?u=haowong
                                    //https://forum.chirpstack.io/t/is-it-normal-to-send-the-unconfirmed-message-once-and-receive-twice/10886/2?u=shirou_emiya

                                    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                                        rxpk_array[loopcount].setDevAddr_get(buffer_array[loopcount].uint, buffer_array->buff_index);
                                        rxpk_array[loopcount].setTime(buffer_array[loopcount].uint, buffer_array->buff_index);
                                        rxpk_array[loopcount].setFcnt(buffer_array[loopcount].uint, buffer_array->buff_index);
                                        rxpk_array[loopcount].setStat(buffer_array[loopcount].uint, buffer_array->buff_index);
                                        rxpk_array[loopcount].setCrc_get(buffer_array[loopcount].uint, buffer_array->buff_index);
                                        rxpk_array[loopcount].setStr(buffer_array[loopcount].uint, buffer_array->buff_index);
                                        rxpk_array[loopcount].setRssi(buffer_array[loopcount].uint, buffer_array->buff_index);
                                        rxpk_array[loopcount].setSNR(buffer_array[loopcount].uint, buffer_array->buff_index);
                                        rxpk_array[loopcount].setPayloadSize(buffer_array[loopcount].uint, buffer_array->buff_index);

#if DEBUG
                                        printf("\nrxpk%d.DevAddr_get: %d\n", loopcount + 1, rxpk_array[loopcount].DevAddr_get);
                                        printf("rxpk%d.stat: %d\n", loopcount + 1, rxpk_array[loopcount].stat);
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
#if DEBUG
                                        printf("Processed CRC%d: %s\n", loopcount + 1, rxpk_array[loopcount].crc);
#endif
                                        char *ptr;
                                        rxpk_array[loopcount].crc_hex = strtoul(rxpk_array[loopcount].crc, &ptr, 16);
#if DEBUG
                                        printf("Processed CRC%d: 0x%04X\n", loopcount + 1, rxpk_array[loopcount].crc_hex);
#endif

                                        rxpk_array[loopcount].DevAddr = new char[BUF_SIZE];
                                        memset(rxpk_array[loopcount].DevAddr, 0, BUF_SIZE * sizeof(char));

                                        sprintf(rxpk_array[loopcount].DevAddr, "0x%08X", rxpk_array[loopcount].DevAddr_get);
#if DEBUG
                                        printf("Processed DevAddr%d: %s\n", loopcount + 1, rxpk_array[loopcount].DevAddr);
#endif
                                        rxpk_array[loopcount].DevAddr_hex = strtoul(rxpk_array[loopcount].DevAddr, &ptr, 16);
#if DEBUG
                                        printf("Processed DevAddr%d: 0x%08X\n", loopcount + 1, rxpk_array[loopcount].DevAddr_hex);
#endif
                                    }

                                    /* -------------------------------------------------------------------------- */
                                    /* --- STAGE : 当全部上行数据都错且crc值相同时进行纠错 ---------------------- */

                                    if (compareStat(rxpk_array, buffer_num)) {

                                        CRCErrorNumBefore++;

                                        if (compareDevAddr(rxpk_array, buffer_num)) {

                                            if (compareCRC(rxpk_array, buffer_num)) {

                                                printf("/* ----------------------Error correction begins--------------------------------- */\n");

                                                /* -------------------------------------------------------------------------- */
                                                /* --- STAGE : Decoding ---------------------- */

                                                for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                                                    memset(buffer_array[loopcount].payload, 0, BUF_SIZE * sizeof(uint8_t));

                                                    buffer_array[loopcount].setSize(rxpk_array[loopcount].str);//与net_downlink相似，都是接收到data，故都用b64_to_bin
#if DEGUG
                                                    cout << "copy" << loopcount + 1 << " of data: " << rxpk_array[loopcount].str << endl;
#endif
                                                }

                                                uint16_t size;

                                                if ((buffer_array[0].size == buffer_array[1].size) && (buffer_array[0].size == buffer_array[2].size) && (buffer_array[0].size == buffer_array[3].size)) {
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
                                                    logCRC(rxpk_array[loopcount].crc);
                                                }

                                                logLine();

                                                /* -------------------------------------------------------------------------- */
                                                /* --- STAGE : 纠错 ---------------------- */

                                                char *mch = new char[BUF_SIZE];
                                                memset(mch, 0, BUF_SIZE * sizeof(char));

                                                int index = compareSNR(rxpk_array, buffer_num);//Selection Combining (SC)
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
                                                            getFourthNe(buffer_array[0].payload, buffer_array[1].payload, buffer_array[2].payload, buffer_array[3].payload, size, Hamming_weight_now);//Calculate Hamming weight

                                                            if (Hamming_weight_now > Hamming_weight_max) {

                                                                printf("%s: %d\n", "Hamming weight is larger than the max number", Hamming_weight_max);
                                                                printf("This program will be shut down!\n");
                                                                printf("/* ----------------------Error correction ends--------------------------------- */\n\n");
                                                                CRCErrorNumAfter++;
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
                                                                    incremental_correct(buffer.Binarystring, mch, Hamming_weight_now, rxpk_array[0].crc_get, fakeresult, realresult, size, pass_crc, total_number, startTime, rxpk_array[index].fcnt, rxpk_array[index].DevAddr_hex);
                                                                } else {
                                                                    correct(buffer.Binarystring, mch, Hamming_weight_now, rxpk_array[0].crc_get, fakeresult, realresult, size, pass_crc, total_number, startTime, rxpk_array[index].fcnt, rxpk_array[index].DevAddr_hex);
                                                                }
                                                            } else if (compareCRC3(rxpk_array)) {
                                                                if (Hamming_weight_now <= Hamming_weight_max / 2) {
                                                                    incremental_correct(buffer.Binarystring, mch, Hamming_weight_now, compareCRC3(rxpk_array), fakeresult, realresult, size, pass_crc, total_number, startTime, rxpk_array[index].fcnt, rxpk_array[index].DevAddr_hex);
                                                                } else {
                                                                    correct(buffer.Binarystring, mch, Hamming_weight_now, compareCRC3(rxpk_array), fakeresult, realresult, size, pass_crc, total_number, startTime, rxpk_array[index].fcnt, rxpk_array[index].DevAddr_hex);
                                                                }
                                                            } else {
                                                                for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                                                                    if (Hamming_weight_now <= Hamming_weight_max / 2) {
                                                                        incremental_correct(buffer.Binarystring, mch, Hamming_weight_now, rxpk_array[loopcount].crc_get, fakeresult, realresult, size, pass_crc, total_number, startTime, rxpk_array[index].fcnt, rxpk_array[index].DevAddr_hex);
                                                                    } else {
                                                                        correct(buffer.Binarystring, mch, Hamming_weight_now, rxpk_array[loopcount].crc_get, fakeresult, realresult, size, pass_crc, total_number, startTime, rxpk_array[index].fcnt, rxpk_array[index].DevAddr_hex);
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
                                                                CRCErrorNumAfter++;
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
                                                                validateCRC(rxpk_array[0].crc_get, buffer.Binarystring3, realresult[0], size, pass_crc, rxpk_array[index].fcnt, rxpk_array[index].DevAddr_hex);
                                                            } else if (compareCRC3(rxpk_array)) {
                                                                validateCRC(compareCRC3(rxpk_array), buffer.Binarystring3, realresult[0], size, pass_crc, rxpk_array[index].fcnt, rxpk_array[index].DevAddr_hex);
                                                            } else {
                                                                for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                                                                    validateCRC(rxpk_array[loopcount].crc_get, buffer.Binarystring3, realresult[0], size, pass_crc, rxpk_array[index].fcnt, rxpk_array[index].DevAddr_hex);
                                                                }
                                                            }

                                                            if (strlen(*realresult) == 0) {
                                                                printf("%s\n", "Error can not be fixed! APC continues!");

                                                                if (compareCRC2(rxpk_array, buffer_num)) {
                                                                    if (Hamming_weight_now <= Hamming_weight_max / 2) {
                                                                        incremental_correct(buffer.Binarystring2, mch, Hamming_weight_now, rxpk_array[0].crc_get, fakeresult, realresult, size, pass_crc, total_number, startTime, rxpk_array[index].fcnt, rxpk_array[index].DevAddr_hex);
                                                                    } else {
                                                                        correct(buffer.Binarystring2, mch, Hamming_weight_now, rxpk_array[0].crc_get, fakeresult, realresult, size, pass_crc, total_number, startTime, rxpk_array[index].fcnt, rxpk_array[index].DevAddr_hex);
                                                                    }
                                                                } else if (compareCRC3(rxpk_array)) {
                                                                    if (Hamming_weight_now <= Hamming_weight_max / 2) {
                                                                        incremental_correct(buffer.Binarystring2, mch, Hamming_weight_now, compareCRC3(rxpk_array), fakeresult, realresult, size, pass_crc, total_number, startTime, rxpk_array[index].fcnt, rxpk_array[index].DevAddr_hex);
                                                                    } else {
                                                                        correct(buffer.Binarystring2, mch, Hamming_weight_now, compareCRC3(rxpk_array), fakeresult, realresult, size, pass_crc, total_number, startTime, rxpk_array[index].fcnt, rxpk_array[index].DevAddr_hex);
                                                                    }
                                                                } else {
                                                                    for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                                                                        if (Hamming_weight_now <= Hamming_weight_max / 2) {
                                                                            incremental_correct(buffer.Binarystring2, mch, Hamming_weight_now, rxpk_array[loopcount].crc_get, fakeresult, realresult, size, pass_crc, total_number, startTime, rxpk_array[index].fcnt, rxpk_array[index].DevAddr_hex);
                                                                        } else {
                                                                            correct(buffer.Binarystring2, mch, Hamming_weight_now, rxpk_array[loopcount].crc_get, fakeresult, realresult, size, pass_crc, total_number, startTime, rxpk_array[index].fcnt,rxpk_array[index].DevAddr_hex);
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

                                                            softDecoding(buffer_array[0].Binarystring, buffer_array[1].Binarystring, buffer_array[2].Binarystring, buffer_array[3].Binarystring, buffer.Binarystring4, rxpk_array[0].snr, rxpk_array[1].snr, rxpk_array[2].snr, rxpk_array[3].snr);

                                                            if (compareCRC2(rxpk_array, buffer_num)) {
                                                                validateCRC(rxpk_array[0].crc_get, buffer.Binarystring4, realresult[0], size, pass_crc, rxpk_array[index].fcnt, rxpk_array[index].DevAddr_hex);
                                                            } else if (compareCRC3(rxpk_array)) {
                                                                validateCRC(compareCRC3(rxpk_array), buffer.Binarystring4, realresult[0], size, pass_crc, rxpk_array[index].fcnt, rxpk_array[index].DevAddr_hex);
                                                            } else {
                                                                for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                                                                    validateCRC(rxpk_array[loopcount].crc_get, buffer.Binarystring4, realresult[0], size, pass_crc, rxpk_array[index].fcnt, rxpk_array[index].DevAddr_hex);
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
                                                        printf("/* ----------------------Error correction ends--------------------------------- */\n\n");
                                                        continue;
                                                    }
                                                }


                                                struct timespec endTime;
                                                clock_gettime(CLOCK_MONOTONIC, &endTime);

                                                printf("INFO: [up] Total time use in %i ms\n", (int) (1000 * difftimespec(endTime, startTime)));

                                                for (int loopcount = 0; loopcount <= buffer_num - 1; loopcount++) {
                                                    delete[] buffer_array[loopcount].Binarystring;
                                                    delete[] rxpk_array[loopcount].crc;
                                                    delete[] rxpk_array[loopcount].DevAddr;
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

                                                    struct timespec ProEndTime;
                                                    clock_gettime(CLOCK_REALTIME, &ProEndTime);

                                                    printf("INFO: [up] Program total time use in %i ms\n", (int) (1000 * difftimespec(ProEndTime, ProStartTime)));

                                                    cout << "Program throughoutData: " << throughoutData << " Bytes" << endl;
                                                    throughout = 1000 * double((throughoutData * 8 / 1000) / (int) (1000 * difftimespec(ProEndTime, ProStartTime)));
                                                    cout << "Program throughout: " << throughout << " kbps" << endl;
                                                } else {
                                                    CRCErrorNumAfter++;
                                                }

                                                CorrectedFlag = false;//重新初始化Flag

                                                PERAfter = CRCErrorNumAfter / (CRCErrorNumAfter + NonCRCErrorNumAfter);
                                                PDRAfter = 1 - PERAfter;
                                                printf("Packet error rate After: %f\n", PERAfter);
                                                printf("Packet delivery rate After: %f\n", PDRAfter);

                                                printf("/* ----------------------Error correction ends--------------------------------- */\n\n");


                                            } else {

                                                printf("/* ----------------------Special case begins--------------------------------- */\n");

                                                CRCErrorNumAfter++;
                                                PERAfter = CRCErrorNumAfter / (CRCErrorNumAfter + NonCRCErrorNumAfter);
                                                PDRAfter = 1 - PERAfter;
                                                printf("Packet error rate After: %f\n", PERAfter);
                                                printf("Packet delivery rate After: %f\n", PDRAfter);

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

                                            CRCErrorNumAfter++;
                                            PERAfter = CRCErrorNumAfter / (CRCErrorNumAfter + NonCRCErrorNumAfter);
                                            PDRAfter = 1 - PERAfter;
                                            printf("Packet error rate After: %f\n", PERAfter);
                                            printf("Packet delivery rate After: %f\n", PDRAfter);

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

                                        NonCRCErrorNumBefore++;
                                        PERBefore = CRCErrorNumBefore / (CRCErrorNumBefore + NonCRCErrorNumBefore);
                                        PDRBefore = 1 - PERBefore;
                                        printf("Packet error rate Before: %f\n", PERBefore);
                                        printf("Packet delivery rate Before: %f\n", PDRBefore);

                                        NonCRCErrorNumAfter++;
                                        PERAfter = CRCErrorNumAfter / (CRCErrorNumAfter + NonCRCErrorNumAfter);
                                        PDRAfter = 1 - PERAfter;
                                        printf("Packet error rate After: %f\n", PERAfter);
                                        printf("Packet delivery rate After: %f\n", PDRAfter);

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

                                        printf("INFO: [up] Program total time use in %i ms\n", (int) (1000 * difftimespec(ProEndTime, ProStartTime)));

                                        cout << "Program throughoutData: " << throughoutData << " Bytes" << endl;
                                        throughout = 1000 * double((throughoutData * 8 / 1000) / (int) (1000 * difftimespec(ProEndTime, ProStartTime)));
                                        cout << "Program throughout: " << throughout << " kbps" << endl;

                                        printf("/* ----------------------Special case ends--------------------------------- */\n\n");
                                    }
                                }
                            } break;
                        }
                    } else if (byte_num < 0) {
                        printf("rescessed error!");
                    }

                    //某个客户端退出
                    else//cancel fdset and set fd=0
                    {
                        //printf("clien[%d] exit!\n", i);
                        FD_CLR(client_fds[i], &ser_fdset);
                        client_fds[i] = 0;
                        // printf("clien[%d] exit!\n",i);
                        continue;//这里如果用break的话一个客户端退出会造成服务器也退出。
                    }
                }
            }
        }
    }
    return 0;
}