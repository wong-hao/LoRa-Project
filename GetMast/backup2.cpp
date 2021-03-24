
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>        /* C99 types */
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <stdio.h>
#include <algorithm>

void PositiveIntToBinary4(long value, int bits, char* binaryreturn)
{
    //char binary[bits] = "";
    char *binary=new char[bits]; //https://blog.csdn.net/weixin_42638401/article/details/88957796
    int index = 0;

    while (index < bits)
    {
        value % 2 ? binary[index++] = '1' : binary[index++] = '0';
        value /= 2;
    }
    if (value)
    {
        printf("error: over the limit!");
        return;
    }

    //for(int i=bits-1;i>=0;i--)
    //    printf("%c",binary[i]);     

    for (int i = 0; i < bits; i++)//��a[]����ֵ��b[]: https://blog.csdn.net/jamenu/article/details/83786716
    {
        binaryreturn[i] = binary[bits - i - 1];
    }

    delete[] binary;
}


int main1()
{

/* -------------------------------------------------------------------------- */
/* --- STAGE 1: uint8_tתchar ---------------------- */ //https://bbs.csdn.net/topics/390141308
    uint8_t m[256] = "9a9a9a"; //PHYPayload
    uint8_t n[256] = "565656";

    char* m1 = (char*)m;
    char* n1 = (char*)n; 


/* -------------------------------------------------------------------------- */
/* --- STAGE 2: charת��Ϊint ---------------------- */ //https://www.cnblogs.com/xingboy/p/9707052.html
    char* str;
    long a = strtol(m1, &str, 16);  //TODO: longҪ�ĳ�double������ֻ��4Byte
    long b = strtol(n1, &str, 16);

    //printf("%d\n", a);

/* -------------------------------------------------------------------------- */
/* --- STAGE 3: ʮ����int XOR ʮ����int ---------------------- */ //https://zhidao.baidu.com/question/550030489.html
    long c = a ^ b;
    
    //int a=0x9a, b=0x56; 
    //int c = a^b; // ��a��b�����������
    //a = 10011010; // 0x9a
    //b = 01010110; // 0x56
    //c = 11001100 // 0xcc (��Ӧ�Ķ�����λ�����������)
    
    //printf("%d\n", c);

/* -------------------------------------------------------------------------- */
/* --- STAGE 4: ʮ����intת������int ---------------------- */ //https://www.jianshu.com/p/851410c8a296 (itoa�����ã�cǰ���0��û��)

    //printf("length: %d\n", strlen(m1) * 4);
    char d[256] = "";
    PositiveIntToBinary4(c, strlen(m1) * 4, d);
    printf("%s\n", d);

    //long e = atol(d); //https://blog.csdn.net/u013203733/article/details/73869425?utm_medium=distribute.pc_relevant.none-task-blog-BlogCommendFromMachineLearnPai2-4.control&dist_request_id=1328655.327.16158095095668225&depth_1-utm_source=distribute.pc_relevant.none-task-blog-BlogCommendFromMachineLearnPai2-4.control
    //printf("%d", e);  //�����������������
    return 0;
}