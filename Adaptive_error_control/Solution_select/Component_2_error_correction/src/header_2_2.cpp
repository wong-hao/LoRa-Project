#include "header_2_2.h"
#include "header_1_1.h"

FILE *outfile;
char fileName[BUF_SIZE] = "-PHYPayload.";
char fileType[BUF_SIZE] = "csv";

void initFile(char *input) {
    //写数据
    strcat(input, fileName);
    strcat(input, fileType);
    memset(fileName, 0, BUF_SIZE * sizeof(char));
    strcpy(fileName, input);
    outfile = fopen(fileName, "a");

    if (outfile == nullptr) {
        printf("Can't open the file!\n");
    }
    fprintf(outfile, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", "CRC1", "CRC2", "CRC3", "CRC4", "PHYPayload1", "Fcnt1", "PHYPayload2", "Fcnt2", "PHYPayload3", "Fcnt3", "PHYPayload4", "Fcnt4", "PDR", "Time(ms)", "ThroughoutData(Byte)", "Throughout(kbp)");
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

void logFcnt(int input) {
    fprintf(outfile, "%d,", input);
}

void logPDRA(double input) {
    fprintf(outfile, "%f, ", input);
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
