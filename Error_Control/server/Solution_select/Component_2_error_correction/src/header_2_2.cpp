#include "header_2_2.h"
#include "header_1_1.h"

struct timespec ProStartTime;

FILE *outfile;//全局文件
const char *dir_path = "./ReLoRaWAN_CS_bin/";
char fileName[BUF_SIZE] = "-Dataset.";
char fileType[BUF_SIZE] = "csv";

FILE *errorfile;//仅发生错误文件
char errorName[BUF_SIZE] = "-ErrorDataset.";

FILE *wholefile;//全过程文件
char wholeName[BUF_SIZE] = "-WholeDataset.";
/* -------------------------------------------------------------------------- */
/* --- Overall File log function ---------------------- */

void initFile() {
    clock_gettime(CLOCK_REALTIME, &ProStartTime);
    struct tm t;
    char date_time[BUF_SIZE];

    char file_loc[BUF_SIZE];
    strcpy(file_loc, dir_path);

    strftime(date_time, sizeof(date_time), "%Y-%m-%d-%H-%M-%S",
             localtime_r(&ProStartTime.tv_sec, &t));

    //写数据
    strcat(file_loc, date_time);
    strcat(file_loc, fileName);
    strcat(file_loc, fileType);
    memset(fileName, 0, BUF_SIZE * sizeof(char));
    strcpy(fileName, file_loc);
    outfile = fopen(fileName, "a");

    if (outfile == nullptr) {
        printf("Can't open the file!\n");
    }
    fprintf(outfile, "%s,%s,%s,%s\n", "PDR", "time", "TotalTime(ms)", "Throughput(kbp)");
    fclose(outfile);
}

void openFile() {
    //写数据
    outfile = fopen(fileName, "a");
    if (outfile == nullptr) {
        printf("Can't open the file!\n");
    }
}

void logCRC(char *input) {
    fprintf(outfile, "%s,", input);
}

void logPHYPayload(uint8_t *input, int size) {
    char *string = new char[BUF_SIZE];
    memset(string, 0, BUF_SIZE * sizeof(char));

    Uint2Char(input, string, size);
    fprintf(outfile, "%s,", string);
}

void logData(const char *input) {
    fprintf(outfile, "%s,", input);
}

void logFcnt(int input) {
    fprintf(outfile, "%d,", input);
}

void logPDRA(double input) {
    fprintf(outfile, "%f, ", input);
}

void logTimestamp(struct timespec endTime) {
    struct tm t;

    char date_time[BUF_SIZE];

    strftime(date_time, sizeof(date_time), "%Y-%m-%dT%XZ",
             localtime_r(&endTime.tv_sec, &t));

    fprintf(outfile, "%s, ", date_time);
}

void logTime(int input) {
    fprintf(outfile, "%d, ", input);
}

void logThroughputData(double input) {
    fprintf(outfile, "%f, ", input);
}

void logThroughput(double input) {
    fprintf(outfile, "%f", input);
}

void logLine() {
    fprintf(outfile, "\n");
    fclose(outfile);
}

/* -------------------------------------------------------------------------- */
/* --- Error File log function ---------------------- */

void initErrorFile() {
    clock_gettime(CLOCK_REALTIME, &ProStartTime);
    struct tm t;
    char date_time[BUF_SIZE];

    char file_loc[BUF_SIZE];
    strcpy(file_loc, dir_path);

    strftime(date_time, sizeof(date_time), "%Y-%m-%d-%H-%M-%S",
             localtime_r(&ProStartTime.tv_sec, &t));

    //写数据
    strcat(file_loc, date_time);
    strcat(file_loc, errorName);
    strcat(file_loc, fileType);
    memset(errorName, 0, BUF_SIZE * sizeof(char));
    strcpy(errorName, file_loc);
    errorfile = fopen(errorName, "a");

    if (errorfile == nullptr) {
        printf("Can't open the file!\n");
    }
    fprintf(errorfile, "%s, %s\n", "Hamming_weight", "Result");
    fclose(errorfile);
}

void openErrorFile() {
    //写数据
    errorfile = fopen(errorName, "a");
    if (errorfile == nullptr) {
        printf("Can't open the file!\n");
    }
}

void logHammingWeight(double input) {
    fprintf(errorfile, "%f, ", input);
}

void logResult(bool input) {
    fprintf(errorfile, "%d", input);
}

void logErrorLine() {
    fprintf(errorfile, "\n");
    fclose(errorfile);
}

/* -------------------------------------------------------------------------- */
/* --- Whole File log function ---------------------- */

void initWholeFile() {
    clock_gettime(CLOCK_REALTIME, &ProStartTime);
    struct tm t;
    char date_time[BUF_SIZE];

    char file_loc[BUF_SIZE];
    strcpy(file_loc, dir_path);

    strftime(date_time, sizeof(date_time), "%Y-%m-%d-%H-%M-%S",
             localtime_r(&ProStartTime.tv_sec, &t));

    //写数据
    strcat(file_loc, date_time);
    strcat(file_loc, wholeName);
    strcat(file_loc, fileType);
    memset(wholeName, 0, BUF_SIZE * sizeof(char));
    strcpy(wholeName, file_loc);
    wholefile = fopen(wholeName, "a");

    if (wholefile == nullptr) {
        printf("Can't open the file!\n");
    }
    fprintf(wholefile, "%s, %s, %s, %s, %s, %s, %s, %s, %s, %s\n", "stat1", "stat2", "stat3", "stat4", "data1", "data2", "data3", "data4", "Result", "time");
    fclose(wholefile);
}

void openWholeFile() {
    //写数据
    wholefile = fopen(wholeName, "a");
    if (wholefile == nullptr) {
        printf("Can't open the file!\n");
    }
}

void logStat(int input) {
    fprintf(wholefile, "%d,", input);
}

void logDataWhole(const char *input) {
    fprintf(wholefile, "%s,", input);
}

void logResultWhole(bool input) {
    fprintf(wholefile, "%d, ", input);
}

void logTimestampWhole(struct timespec endTime) {
    struct tm t;

    char date_time[BUF_SIZE];

    strftime(date_time, sizeof(date_time), "%Y-%m-%dT%XZ",
             localtime_r(&endTime.tv_sec, &t));

    fprintf(wholefile, "%s", date_time);
}

void logWholeLine() {
    fprintf(wholefile, "\n");
    fclose(wholefile);
}