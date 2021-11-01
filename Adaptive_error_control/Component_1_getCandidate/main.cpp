//https://blog.csdn.net/zhao2018/article/details/82803070?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522163133865416780264014767%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=163133865416780264014767&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~rank_v29_ecpm-1-82803070.pc_search_result_cache&utm_term=C%2B%2B%E7%A9%B7%E4%B8%BEn%E4%BD%8D%E4%BA%8C%E8%BF%9B%E5%88%B6%E6%95%B0&spm=1018.2226.3001.4187
//https://blog.csdn.net/sayniceoh/article/details/53105466?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522163133865416780264014767%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=163133865416780264014767&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~rank_v29_ecpm-3-53105466.pc_search_result_cache&utm_term=C%2B%2B%E7%A9%B7%E4%B8%BEn%E4%BD%8D%E4%BA%8C%E8%BF%9B%E5%88%B6%E6%95%B0&spm=1018.2226.3001.4187
//https://www.codeleading.com/article/3956123395/
//https://blog.csdn.net/acsuperman/article/details/79930557
//https://www.geek-share.com/detail/2720457588.html

#include "header_1_2.h"


int main() {

    char Binarystring3[20] = "01111";

    int Hamming_weight_max = 30;
    int Hamming_weight_now = 4;

    struct timespec startTime;
    clock_gettime(CLOCK_REALTIME, &startTime);

    if (Hamming_weight_now < Hamming_weight_max / 2) {
        incremental_correct(Binarystring3, Hamming_weight_now, startTime);
    } else {
        correct(Binarystring3, Hamming_weight_now, startTime);
    }

    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &endTime);

    struct timespec interv;
    diff(&startTime, &endTime, &interv);
    cout << "Total timeuse: " << double(interv.tv_sec * NANOSECOND + interv.tv_nsec) / NANOSECOND << "s" << endl;

    return 0;
}
