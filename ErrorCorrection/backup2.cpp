//�Լ�д��

#include <stdio.h>
#include <string.h> //ʮ�������ַ���ת�������ַ���
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

    int one = 0; //����λ����

    char* zerolocation = new char[length+1]; //��λλ��



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

    printf("����λ����: %d\n", one);

    
 

    delete[] zerolocation;
    return 0;
}
