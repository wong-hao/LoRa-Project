//https://zhidao.baidu.com/question/208360934.html?qq-pf-to=pcqq.group
#include <stdio.h>
#include <string.h> //十六进制字符串转二进制字符串
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> /* C99 types */
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <stdio.h>
#include <algorithm>
int main() {
    int row = 0;    //函数
    int col = 0;    //列数
    int i;
    int** arr = NULL;
    printf("请输入二维数组的行数和列数:");
    scanf("%d%d", &row, &col);

    arr = (int**)malloc(sizeof(int*) * row);
    for (i = 0; i < row; i++) {
        arr[i] = (int*)malloc(sizeof(int) * col);
    }

    for (int i = 0; i < row; i++) {
        for(int j=0;j<col;j++)
            scanf("%d", &arr[i][j]);
    }

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++)
            printf("%d", arr[i][j]);
    }
}