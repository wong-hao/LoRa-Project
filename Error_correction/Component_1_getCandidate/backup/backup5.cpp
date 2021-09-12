//https://blog.csdn.net/zhao2018/article/details/82803070?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522163133865416780264014767%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=163133865416780264014767&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~rank_v29_ecpm-1-82803070.pc_search_result_cache&utm_term=C%2B%2B%E7%A9%B7%E4%B8%BEn%E4%BD%8D%E4%BA%8C%E8%BF%9B%E5%88%B6%E6%95%B0&spm=1018.2226.3001.4187
//https://blog.csdn.net/sayniceoh/article/details/53105466?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522163133865416780264014767%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=163133865416780264014767&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~rank_v29_ecpm-3-53105466.pc_search_result_cache&utm_term=C%2B%2B%E7%A9%B7%E4%B8%BEn%E4%BD%8D%E4%BA%8C%E8%BF%9B%E5%88%B6%E6%95%B0&spm=1018.2226.3001.4187

#include <iostream>
#include <stdio.h>
#include <cstring>
using namespace std;

char num[20];
int n;

void countone(char* input, int* count){
    for (int i = 0; i < strlen(input); i++) {
        if (input[i] == '1') (*count)++;
    }

}

void insertzero(char* input, int location){

    string input_str(input);
    input_str.insert(location,"0");
    strcpy(input,input_str.c_str());

}

void Search(char* input, int m)
{
    int i;
    if(m == n)
    {
        //printf("%s",num);
        char* num2 = new char [strlen(num)];
        memset(num2, 0, (strlen(num2)) * sizeof(char));
        strcpy(num2,num);

        for(int j=0;j<= strlen(input)-1;j++){
            if(input[j]=='0'){
                insertzero(num2,j);
            }
        }

        printf("%s",num2);
        char parr[] = "001";

        if(strcmp(num2,parr)==0){
            printf("Yes");
        }
        printf("\n");
    }
    else
    {
        num[m]='0';Search(input, m+1);
        num[m]='1';Search(input, m+1);
    }
}


void input(char* input) {
    int one = 0;

    countone(input,&one);

    n = one;
    int m = 0;
    Search(input, m);
}



int main()
{

    char x[20];
    cin >> x;
    input(x);


    return 0;
}


