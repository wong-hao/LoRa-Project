//自己写的

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
    char Binarystring[] = "010100000111";
    int length = strlen(Binarystring);

    //printf("%d\n", length);

    int one = 0; //非零位个数

    char* zerolocation = new char[length+1]; //零位位置



    for (int i = 0; i < length; i++) {
        zerolocation[i] = '1';
    }

    puts(zerolocation);

    printf("%d\n", strlen(zerolocation));
    
    for (int i = 0; i < length; i++) {
        if (Binarystring[i] == '1') {
            one++;
        }
        else {
            zerolocation[i] = '1';
        }
    }

    printf("非零位个数: %d\n", one);

    
 

    delete[] zerolocation;
    return 0;
}
