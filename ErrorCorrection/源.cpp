//https://bbs.csdn.net/topics/399153127
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
char s[256], d[256]; //s��Merged error mask��d��Error candidate pattern
void outmystr(int n, char* input, char* compare, char* interoutput, char* finaloutput)
{

	if (strcmp(interoutput, compare) == 0) {
		strcpy(finaloutput, interoutput);
		//�����ǰ����һ�����������ĸ�ֵ��realoutput��sPMӦ���Ƿ��ַ�������֮�����Ͻ�������ģ�����return�����þ������ˣ�

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

	int one = 0; //����λ�������������ɶ�ά�ַ�����
	for (int j = 0; j < strlen(s); j++) {
		if (s[j] == '1') {
			one++;
		}
	}

	char* result = new char[strlen(s) + 1];//����copy��һά�ַ�����
	memset(result, 0, strlen(s) + 1);
	*/


	while (s[i])
		d[i++] = '0';

	outmystr(i - 1,mch, crc, fakeresult, realresult);




	return 0;
}
