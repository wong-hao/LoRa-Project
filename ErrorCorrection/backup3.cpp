//https://blog.csdn.net/lydia_ke/article/details/78401002
#include<stdio.h> 
int a[100] = { 0 };
int n; //n������a����Ϊȫ�ֱ������������ݴ��䡣
void print(int a[100])//������������ĺ���
{
    int i;
    for (i = 0; i <= n - 2; i++)
    {
        printf("%d", a[i]);
    }
    printf("%d\n", a[i]);
}
void meiju(int m)//��ٵĺ���
{
    if (m == n)//��Ϊ���ݵ���ֹ�㣬��ʼ�����һλ���ص��ơ�
    {
        print(a);//һ������оٽ���������������
    }
    else if (m < n)
    {
        for (int i = 0; i <= 1; i++)//��i>1ʱͬ����Ϊ������ֹ�㣬��ʼ����һλ���ơ�
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