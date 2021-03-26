//https://bbs.csdn.net/topics/399153127
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
char s[256], d[256]; //s是Merged error mask；d是Error candidate pattern
void outmystr(int n, char* input, char* compare, char* interoutput, char* finaloutput)
{

	if (strcmp(interoutput, compare) == 0) {
		strcpy(finaloutput, interoutput);
		//这里是把最后一个符合条件的赋值给realoutput（sPM应该是发现符合条件之后马上结束程序的，但是return不管用就先算了）

	}

	if (n < 0) {


		printf("Candidate: %s ", d);
		printf("Interoutput: %s\n", interoutput);

	}
	else
	{
		d[n] = '0';
		outmystr(n - 1, input, compare, interoutput, finaloutput);
		if (s[n] == '1')
		{
			d[n] = '1';
			outmystr(n - 1, input, compare, interoutput, finaloutput);
		}

	}

}
int main()
{

	char mch[64] = "1010"; 
	char fakeresult[256] = "";
	char realresult[256] = "";
	char crc[256] = "1010";

	
	int i = 0;
	char Binarystring31[] = "1100";
	strcpy(s, Binarystring31);
	printf("Mask: %s\n", s);

;
	/*

	int one = 0; //非零位个数：用于生成二维字符数组
	for (int j = 0; j < strlen(s); j++) {
		if (s[j] == '1') {
			one++;
		}
	}

	char* result = new char[strlen(s) + 1];//用于copy到一维字符数组
	memset(result, 0, strlen(s) + 1);
	*/


	while (s[i])
		d[i++] = '0';

	outmystr(i - 1,mch, crc, fakeresult, realresult);




	return 0;
}
