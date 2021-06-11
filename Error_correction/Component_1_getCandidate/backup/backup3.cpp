//https://blog.csdn.net/lydia_ke/article/details/78401002
#include<stdio.h> 
int a[100] = { 0 };
int n; //n和数组a声明为全局变量，方便数据传输。
void print(int a[100])//输出二进制数的函数
{
    int i;
    for (i = 0; i <= n - 2; i++)
    {
        printf("%d", a[i]);
    }
    printf("%d\n", a[i]);
}
void meiju(int m)//穷举的函数
{
    if (m == n)//作为回溯的终止点，开始从最后一位往回递推。
    {
        print(a);//一次情况列举结束，输出这种情况
    }
    else if (m < n)
    {
        for (int i = 0; i <= 1; i++)//当i>1时同样作为回溯终止点，开始往上一位递推。
        {
            a[m] = i;
            meiju(m + 1);
        }
    }
}
int main()
{
    scanf("%d", &n);
    meiju(0);
    return 0;
}