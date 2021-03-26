//https://bbs.csdn.net/topics/399153127
#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_INPUT_LEN   32

int makeLable(char* input, int* lable)
{
    int i = 0;
    while (input != NULL && *input != '\0') {
        if (*input == '0') {
            lable[i++] = 0;
        }
        else if (*input == '1') {
            lable[i++] = 1;
        }
        else {
            printf("ERROR: invalid input\n");
            return -1;
        }
        ++input;
    }
    return 0;
}

int getOneNum(int* arr, int len)
{
    int cnt = 0;
    for (int i = 0; i < len; ++i) {
        if (arr[i] == 1)
            cnt++;
    }
    return cnt;
}
void int2arr(int num, int* arr, int len)
{
    int mask = 1;
    for (int i = 0; i < len; ++i) {
        if (0 != (int)(num & mask)) {
            arr[i] = 1;
        }
        else {
            arr[i] = 0;
        }
        mask <<= 1;
    }
}
void getResult(int* lable, int len, int* arr, int oneNum, int* result)
{
    int oneIndex = 0;
    for (int i = 0; i < len; ++i) {
        result[i] = lable[i] == 1 ? arr[oneIndex++] : 0;
    }
}
int main()
{
    char input[MAX_INPUT_LEN + 1] = { 0 };
    fgets(input, MAX_INPUT_LEN, stdin);
    int inlen = strlen(input);
    if ('\n' == input[inlen - 1]) {
        input[inlen - 1] = '\0';
    }
    inlen = strlen(input);

    char* resultchar = new char[inlen];
    printf("input: %s\n", input);
    printf("inlen = %d\n", inlen);
    int lable[MAX_INPUT_LEN] = { 0 };
    if (0 != makeLable(input, lable)) {
        printf("ERROR:make lable error\n");
        return -1;
    }
    int oneNum = getOneNum(lable, inlen);
    printf("oneNum = %d\n", oneNum);

    double max = pow(2, oneNum) - 1;
    printf("max = %f\n", max);

    //int arr[oneNum];
    int* arr = new int[oneNum];
    int result[MAX_INPUT_LEN] = { 0 };
    for (int num = 0; num <= max; ++num) {
        int2arr(num, arr, oneNum);
        getResult(lable, inlen, arr, oneNum, result);
        for (int i = 0; i < inlen; ++i) {
            printf("%d", result[i]);
        }
        printf("\n");
    }
    return 0;
}

