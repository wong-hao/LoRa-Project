#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{

    //int a=0x9a, b=0x56; //https://zhidao.baidu.com/question/550030489.html
    //int c = a^b; // ��a��b�����������
    //a = 10011010; // 0x9a
    //b = 01010110; // 0x56
    //c = 11001100 // 0xcc (��Ӧ�Ķ�����λ�����������)
    //printf("%d", c);

    char m[] = "9a"; //https://www.cnblogs.com/xingboy/p/9707052.html
    char n[] = "56";
    char* str;
    long a = strtol(m, &str, 16);
    long b = strtol(n, &str, 16);
    long c = a ^ b;

    char d[200]; //https://www.jianshu.com/p/851410c8a296
    itoa(c, d, 2);
    //printf("%s",d); //����������ַ���
    long e = atol(d); //https://blog.csdn.net/u013203733/article/details/73869425?utm_medium=distribute.pc_relevant.none-task-blog-BlogCommendFromMachineLearnPai2-4.control&dist_request_id=1328655.327.16158095095668225&depth_1-utm_source=distribute.pc_relevant.none-task-blog-BlogCommendFromMachineLearnPai2-4.control
    printf("%d", e);  //�����������������
    return 0;
}