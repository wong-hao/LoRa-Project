#include "header_1_3.h"

int ranksixfold(int a, int b, int c, int d, int e, int f) {
    int min = a;//默认最大值为a

    if (min > b) min = b;

    if (min > c) min = c;

    if (min > d) min = d;

    if (min > e) min = e;

    if (min > f) min = f;

    return min;
}

void mpc(char* a, char* b, char* c, char* d){

    char* mpc1 = new char[20];
    memset(mpc1, 0, BUF_SIZE*sizeof(char ));
    char* mpc2 = new char[20];
    memset(mpc2, 0, BUF_SIZE*sizeof(char ));
    char* mpc3 = new char[20];
    memset(mpc3, 0, BUF_SIZE*sizeof(char ));
    char* mpc4 = new char[20];
    memset(mpc4, 0, BUF_SIZE*sizeof(char ));
    char* mpc5 = new char[20];
    memset(mpc5, 0, BUF_SIZE*sizeof(char ));
    char* mpc6 = new char[20];
    memset(mpc6, 0, BUF_SIZE*sizeof(char ));

    int Hamming_weight_now1 = 1;
    int Hamming_weight_now2 = 2;
    int Hamming_weight_now3 = 3;
    int Hamming_weight_now4 = 4;
    int Hamming_weight_now5 = 5;
    int Hamming_weight_now6 = 6;


    if (OZ_bin_xor(a, b, mpc1) != 0)
    {
        printf("函数出错！\n");
        return;
    }
    if (OZ_bin_xor(a, c, mpc2) != 0)
    {
        printf("函数出错！\n");
        return;
    }
    if (OZ_bin_xor(a, d, mpc3) != 0)
    {
        printf("函数出错！\n");
        return;
    }
    if (OZ_bin_xor(b, c, mpc4) != 0)
    {
        printf("函数出错！\n");
        return;
    }
    if (OZ_bin_xor(b, d, mpc5) != 0)
    {
        printf("函数出错！\n");
        return;
    }
    if (OZ_bin_xor(c, d, mpc6) != 0)
    {
        printf("函数出错！\n");
        return;
    }

    if(ranksixfold(Hamming_weight_now1, Hamming_weight_now2, Hamming_weight_now3, Hamming_weight_now4, Hamming_weight_now5, Hamming_weight_now6) == Hamming_weight_now1){
        printf("Prcess a");
    }else if(ranksixfold(Hamming_weight_now1, Hamming_weight_now2, Hamming_weight_now3, Hamming_weight_now4, Hamming_weight_now5, Hamming_weight_now6) == Hamming_weight_now2){
        printf("Prcess b");
    }else if(ranksixfold(Hamming_weight_now1, Hamming_weight_now2, Hamming_weight_now3, Hamming_weight_now4, Hamming_weight_now5, Hamming_weight_now6) == Hamming_weight_now3){
        printf("Prcess c");
    }else if(ranksixfold(Hamming_weight_now1, Hamming_weight_now2, Hamming_weight_now3, Hamming_weight_now4, Hamming_weight_now5, Hamming_weight_now6) == Hamming_weight_now4){
        printf("Prcess d");
    }else if(ranksixfold(Hamming_weight_now1, Hamming_weight_now2, Hamming_weight_now3, Hamming_weight_now4, Hamming_weight_now5, Hamming_weight_now6) == Hamming_weight_now5){
        printf("Prcess e");
    }else {
        printf("Prcess f");
    }
}
