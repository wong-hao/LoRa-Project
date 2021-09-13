#include "header_1_3.h"

void majority(char* input1, char* input2, char* input3, char* output){
    if (strlen(input1)!= strlen(input2) || strlen(input1)!= strlen(input3) || strlen(input2)!= strlen(input3)){
        printf("Length is not equal! Program shut down!\n");
        return;
    }

    int size = strlen(input1);

    for(int i=0; i<=size-1; i++){
        if((input1[i]=='1')&&(input2[i]=='1')&&(input3[i]=='1')) {
            output[i] = '1';
        }else{
            output[i] = '0';
        }
    }

}
