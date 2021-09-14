#include "header_1_3.h"

int OZ_bin_and(const char* s1, char* s2, char* dest)
{
    unsigned int i;
    int temp1 = 0, temp2 = 0, temp3 = 0;
    if (strlen(s1) != strlen(s2))
    {
        printf("错误，不等长！\n");
        return 1;
    }
    for (i = 0; i < strlen(s1); i++)
    {
        temp1 = s1[i] - '0';
        temp2 = s2[i] - '0';
        temp3 = temp1 & temp2;
        if (temp3 == 1)
            dest[i] = '1';
        else if (temp3 == 0)
            dest[i] = '0';
        else
        {
            printf("字符串内容有误！\n");
            return 1;
        }
    }
    return 0;
}

void majority(char* input1, char* input2, char* input3, char* output){
    if (strlen(input1)!= strlen(input2) || strlen(input1)!= strlen(input3) || strlen(input2)!= strlen(input3)){
        printf("Length is not equal! Program shut down!\n");
        return;
    }

    if (OZ_bin_and(input1, input2, output) != 0)
    {
        printf("函数出错！\n");
        return;
    }

    if (OZ_bin_and(output, input3, output) != 0)
    {
        printf("函数出错！\n");
        return;
    }

}
