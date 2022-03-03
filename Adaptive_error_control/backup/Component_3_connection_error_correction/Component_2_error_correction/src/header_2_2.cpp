#include "header_2_2.h"
#include "header_1_1.h"

struct timespec ProStartTime;

FILE *outfile;//全局文件
char fileName[BUF_SIZE] = "-Dataset.";
char fileType[BUF_SIZE] = "csv";

FILE *errorfile;//仅发生错误文件
char errorName[BUF_SIZE] = "-ErrorDataset.";

/* -------------------------------------------------------------------------- */
/* --- Overall File log function ---------------------- */

void initFile() {
    clock_gettime(CLOCK_REALTIME, &ProStartTime);
    struct tm t;
    char date_time[BUF_SIZE];
    strftime(date_time, sizeof(date_time), "%Y-%m-%d-%H-%M-%S",
             localtime_r(&ProStartTime.tv_sec, &t));

    //写数据
    strcat(date_time, fileName);
    strcat(date_time, fileType);
    memset(fileName, 0, BUF_SIZE * sizeof(char));
    strcpy(fileName, date_time);
    outfile = fopen(fileName, "a");

    if (outfile == nullptr) {
        printf("Can't open the file!\n");
    }
    fprintf(outfile, "%s,%s,%s,%s,%s,%s,%s,%s\n", "Data1", "Data2", "Data3", "Data4", "PDR", "time", "TotalTime(ms)", "Throughout(kbp)");
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

void logTimestamp(char *input) {
    fprintf(outfile, "%s, ", input);
}

void logTime(int input) {
    fprintf(outfile, "%d, ", input);
}

void logThroughoutData(double input) {
    fprintf(outfile, "%f, ", input);
}

void logThroughout(double input) {
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
    strftime(date_time, sizeof(date_time), "%Y-%m-%d-%H-%M-%S",
             localtime_r(&ProStartTime.tv_sec, &t));

    //写数据
    strcat(date_time, errorName);
    strcat(date_time, fileType);
    memset(errorName, 0, BUF_SIZE * sizeof(char));
    strcpy(errorName, date_time);
    errorfile = fopen(errorName, "a");

    if (errorfile == nullptr) {
        printf("Can't open the file!\n");
    }
    fprintf(errorfile, "%s\n", "Hamming_weight");
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
    fprintf(errorfile, "%f", input);
}

void logErrorLine() {
    fprintf(errorfile, "\n");
    fclose(errorfile);
}