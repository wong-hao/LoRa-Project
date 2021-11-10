#include "header_2_2.h"
#include "header_1_1.h"

FILE *outfile;

void initFile() {
    //写数据
    outfile = fopen("PHYPayload.csv", "a");
    if (outfile == NULL) {
        printf("Can't open the file!\n");
    }
    fprintf(outfile, "%s,%s,%s,%s,%s,%s,%s,%s\n", "PHYPayload1", "PHYPayload2", "PHYPayload3", "PHYPayload4", "CRC1", "CRC2", "CRC3", "CRC4");
    fclose(outfile);
}

void openFile() {
    //写数据
    outfile = fopen("PHYPayload.csv", "a");
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
