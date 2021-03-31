//https://blog.csdn.net/codedz/article/details/80387001

#include<iostream>
#include<cstdlib>
#include<ctime>
using namespace std;
clock_t startpoint, endpoint;

int main() {
	startpoint = clock();		//程序开始计时
	int ans = 0;
	for (int i = 1; i <= 1e8; i++)
		ans++;
	endpoint = clock();		//程序结束用时
	double endtime = (double)(endpoint - startpoint) / CLOCKS_PER_SEC;
	cout << "Total time:" << endtime << endl;		//s为单位
	cout << "Total time:" << endtime * 1000 << "ms" << endl;	//ms为单位
	system("pause");
	return 0;
}