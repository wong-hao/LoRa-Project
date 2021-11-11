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

    if (outfile == NULL) {
        printf("Can't open the file!\n");
    }
    fprintf(outfile, "%s,%s,%s,%s,%s,%s,%s,%s\n", "PHYPayload1", "PHYPayload2", "PHYPayload3", "PHYPayload4", "CRC1", "CRC2", "CRC3", "CRC4");
    fclose(outfile);
}

void openFile() {
    //写数据
    outfile = fopen(fileName, "a");
    if (outfile == NULL) {
        printf("Can't open the file!\n");
    }
}

void logPHYPayload(char *input) {
    fprintf(outfile, "%s,", input);
}

void logCRC(int input) {
    fprintf(outfile, "%d,", input);
}

void logLine() {
    fprintf(outfile, "\n");
    fclose(outfile);
}
