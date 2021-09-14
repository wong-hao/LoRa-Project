#include "header_1_3.h"

int ranktriple(int a, int b, int c) {
    int min = a;//默认最大值为a

    if (min > b) min = b;

    if (min > c) min = c;

    return min;
}

void mpc(char* a, char* b, char* c){

    char* mpc1 = new char[20];
    memset(mpc1, 0, BUF_SIZE*sizeof(char ));
    char* mpc2 = new char[20];
    memset(mpc2, 0, BUF_SIZE*sizeof(char ));
    char* mpc3 = new char[20];
    memset(mpc3, 0, BUF_SIZE*sizeof(char ));

    if (OZ_bin_xor(a, b, mpc1) != 0) //TODO: Majority voting / more than two copies
    {
        printf("函数出错！\n");
        return;
    }
    if (OZ_bin_xor(b, c, mpc2) != 0) //TODO: Majority voting / more than two copies
    {
        printf("函数出错！\n");
        return;
    }
    if (OZ_bin_xor(c, a, mpc3) != 0) //TODO: Majority voting / more than two copies
    {
        printf("函数出错！\n");
        return;
    }

    if(ranktriple(countone(mpc1), countone(mpc2), countone(mpc3)) == countone(mpc1)){
        printf("Prcess a");
    }else if(ranktriple(countone(mpc1), countone(mpc2), countone(mpc3)) == countone(mpc2)){
        printf("Prcess b");
    }else{
        printf("Prcess c");
    }
}
