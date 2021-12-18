#include "header_1_1.h"
#include "header_2_1.h"
#include "header_3.h"

double TotalPacket = 0;
double ErrorPacket = 0;
double PEREnv;
double PDREnv;

double TotalSNR = 0;
double SNREnv;

int FindFirstSubchar(char *fullchar, const char *subchar) {

    char *buffer = strstr(fullchar, subchar);//用于接受返回值

    if (buffer == NULL) {//说明没有要找的字符串
        return -1;
    } else {                         //说明找到了那个字符串
        return buffer - fullchar + 1;//cde的地址减去abcde的地址+1
    }
}

int FindSecondSubchar(char *fullchar, const char *subchar) {

    char *buffer = strstr(fullchar, subchar);//接收返回值
    int location;

    if (buffer == nullptr) {
        return -1;
    } else {
        location = buffer - fullchar + strlen(subchar);//找到第一个subchar的位置
    }

    char fullchar_two[BUF_SIZE];
    memset(fullchar_two, 0, BUF_SIZE * sizeof(char));
    strncpy(fullchar_two, fullchar + location, strlen(fullchar) - location);//fullchar_two是fullchar删除了第一个subchar剩下的字符串
    fullchar_two[strlen(fullchar_two)] = '\0';

    char *buffer_two = strstr(fullchar_two, subchar);
    if (buffer_two == nullptr) {
        return -1;
    } else {
        return buffer_two - fullchar_two + 1 + location;
    }
}

void deleteChar(char *array, int location)//http://www.360doc.cn/mip/579579350.html?ivk_sa=1024320u
{

    int j, k;

    for (j = k = 0; array[j] != '\0'; j++) {

        if (j != location) {

            array[k++] = array[j];
        }
    }

    array[k] = '\0';
}

int compareDevAddr(Rxpk *rxpk_array, int array_length) {
    int flag = 1;

    for (int i = 0; i <= array_length - 1; i++) {
        if (strcmp(rxpk_array[i].DevAddr, rxpk_array[0].DevAddr) != 0) {
            flag = 0;
        }
    }

    if (DeprecatedOption) {//deprecated because there is chance to correct the devaddr
        return 1;
    }

    return flag;//经测试大多数全部错误的情况下DevAddr仍然正确
}

int compareStat(Rxpk *rxpk_array, int array_length) {
    int flag = 1;

    for (int i = 0; i <= array_length - 1; i++) {
        if (rxpk_array[i].stat != -1) {
            flag = 0;
        } else {
            ErrorPacket++;
        }

        TotalSNR += rxpk_array[i].snr;
    }

    TotalPacket = TotalPacket + array_length;

    PEREnv = ErrorPacket / TotalPacket;
    PDREnv = 1 - PEREnv;

    SNREnv = TotalSNR / TotalPacket;

    printf("/* ----------------------Environment assessment--------------------------------- */\n");
    printf("Packet error rate Environment: %f\n", PEREnv);
    printf("Packet delivery rate Environment: %f\n", PDREnv);
    printf("Average SNR Environment: %.1f db\n", SNREnv);

    return flag;
}

int compareCRC(Rxpk *rxpk_array, int array_length) {
    int flag = 1;

    for (int i = 0; i <= array_length - 1; i++) {
        if (strcmp(rxpk_array[i].crc, rxpk_array[0].crc) != 0) {
            flag = 0;
        }
    }

    if (DeprecatedOption) {//deprecated temporarily
        return 1;          //赌一把当四个错误时，最后一个crc值没有被修改
    }

    return flag;
}

int compareCRC2(Rxpk *rxpk_array, int array_length) {
    int flag = 1;

    for (int i = 0; i <= array_length - 1; i++) {
        if (strcmp(rxpk_array[i].crc, rxpk_array[0].crc) != 0) {
            flag = 0;
        }
    }

    return flag;//赌一把当四个错误时，最后一个crc值没有被修改
}

int compareCRC3(Rxpk *rxpk_array) {

    if ((strcmp(rxpk_array[0].crc, rxpk_array[1].crc) == 0) || (strcmp(rxpk_array[0].crc, rxpk_array[2].crc) == 0) || (strcmp(rxpk_array[0].crc, rxpk_array[3].crc) == 0)) {
        return rxpk_array[0].crc_get;
    } else if ((strcmp(rxpk_array[1].crc, rxpk_array[2].crc) == 0) || (strcmp(rxpk_array[1].crc, rxpk_array[3].crc) == 0)) {
        return rxpk_array[1].crc_get;
    } else if ((strcmp(rxpk_array[2].crc, rxpk_array[3].crc) == 0)) {
        return rxpk_array[2].crc_get;
    } else {
        return 0;
    }
}

int compareRSSI(Rxpk *rxpk_array, int array_length) {
    int max = rxpk_array[0].rssi;
    int index = 0;

    for (int i = 0; i <= array_length - 1; i++) {
        if (rxpk_array[i].rssi > max) {
            max = rxpk_array[i].rssi;
            index = i;
        }
    }

    return index;
}

int compareSNR(Rxpk *rxpk_array, int array_length) {
    float max = rxpk_array[0].snr;
    int index = 0;

    for (int i = 0; i <= array_length - 1; i++) {
        if (rxpk_array[i].snr > max) {
            max = rxpk_array[i].snr;
            index = i;
        }
    }

    return index;
}

/* -------------------------------------------------------------------------- */
/* --- Fake ---------------------- */

void replaceData1(char *buff_up_char) {
    char replacement[] = "4A4A";
    string s1(buff_up_char);
    s1.replace(s1.length() - 30, strlen(replacement), replacement);

    memset(buff_up_char, 0, BUF_SIZE * sizeof(char));

    strcpy(buff_up_char, s1.c_str());
    buff_up_char[strlen(buff_up_char)] = '\0';
}

void replaceData2(char *buff_up_char) {
    char replacement[] = "6B6B";
    string s1(buff_up_char);
    s1.replace(s1.length() - 36, strlen(replacement), replacement);

    memset(buff_up_char, 0, BUF_SIZE * sizeof(char));

    strcpy(buff_up_char, s1.c_str());
    buff_up_char[strlen(buff_up_char)] = '\0';
}

void replaceData3(char *buff_up_char) {
    char replacement[] = "6D";
    string s1(buff_up_char);
    s1.replace(s1.length() - 42, strlen(replacement), replacement);

    memset(buff_up_char, 0, BUF_SIZE * sizeof(char));

    strcpy(buff_up_char, s1.c_str());
    buff_up_char[strlen(buff_up_char)] = '\0';
}

void replaceData4(char *buff_up_char) {
    char replacement[] = "4B4B";
    string s1(buff_up_char);
    s1.replace(s1.length() - 48, strlen(replacement), replacement);

    memset(buff_up_char, 0, BUF_SIZE * sizeof(char));

    strcpy(buff_up_char, s1.c_str());
    buff_up_char[strlen(buff_up_char)] = '\0';
}

void replaceStat(char *buff_up_char) {
    char *buff_up_char1_reversed = new char[BUF_SIZE];
    memset(buff_up_char1_reversed, 0, BUF_SIZE * sizeof(char));

    const char *tag = "223A312C22";
    char *tag_reversed = new char[BUF_SIZE];
    memset(tag_reversed, 0, BUF_SIZE * sizeof(char));
    const char *minus = "2D";
    char *minus_reversed = new char[BUF_SIZE];
    memset(minus_reversed, 0, BUF_SIZE * sizeof(char));

    std::string tag_string(tag);
    reverse(tag_string.begin(), tag_string.end());
    strcpy(tag_reversed, tag_string.c_str());

    std::string minus_string(minus);
    reverse(minus_string.begin(), minus_string.end());

    std::string buff_up_string(buff_up_char);
    reverse(buff_up_string.begin(), buff_up_string.end());
    strcpy(buff_up_char1_reversed, buff_up_string.c_str());

    int position1 = FindFirstSubchar(buff_up_char1_reversed, tag_reversed);
    buff_up_string.insert(position1 + 5, minus_string);
    reverse(buff_up_string.begin(), buff_up_string.end());
    memset(buff_up_char, 0, BUF_SIZE * sizeof(char));
    strcpy(buff_up_char, buff_up_string.c_str());
}