//https://blog.csdn.net/codedz/article/details/80387001

#include<iostream>
#include<cstdlib>
#include<ctime>
using namespace std;
clock_t startpoint, endpoint;

int main() {
	startpoint = clock();		//����ʼ��ʱ
	int ans = 0;
	for (int i = 1; i <= 1e8; i++)
		ans++;
	endpoint = clock();		//���������ʱ
	double endtime = (double)(endpoint - startpoint) / CLOCKS_PER_SEC;
	cout << "Total time:" << endtime << endl;		//sΪ��λ
	cout << "Total time:" << endtime * 1000 << "ms" << endl;	//msΪ��λ
	system("pause");
	return 0;
}