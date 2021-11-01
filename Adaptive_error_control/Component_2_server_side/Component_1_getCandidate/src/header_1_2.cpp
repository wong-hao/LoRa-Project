#include"header_1_1.h"
#include"header_1_2.h"
#include"header_1_3.h"
#include "payload_crc.h"

#include"payload_diff.h"

#include "base64.h"

/* -------------------------------------------------------------------------- */
/* --- Correct ---------------------- */

char num[BUF_SIZE];
char num2[BUF_SIZE];
int n;

void insertzero(char *input, int location) {

    std::string input_str(input);
    input_str.insert(location, "0");
    strcpy(input, input_str.c_str());

}

void Search(char *input, int m, char *mch, int crc_int, char *fakeresult, char *realresult, int length, int &pass_crc,
            int &total_number, struct timeval startTime) {
    struct timeval nowTime;
    gettimeofday(&nowTime, NULL);
    double timeuse = (nowTime.tv_sec - startTime.tv_sec) + (double) (nowTime.tv_usec - startTime.tv_usec) / 1000000.0;
    if (timeuse >= 5) {
        printf("Time exceed!\n");
        return;
    }

    if (pass_crc == 1) {

        return; //pass_crc=1说明已经有一个crc校验通过的了，直接退出，这样会直接根除掉假阳性false positives (Hash碰撞)

    }

    int i;
    if (m == n) {
        //printf("%s",num);
        strcpy(num2, num);

        for (int j = 0; j <= strlen(input) - 1; j++) {
            if (input[j] == '0') {
                insertzero(num2, j);
            }
        }

        //printf("%s\n", num2);

        OZ_bin_xor(mch, num2, fakeresult);
        //printf("%s\n", fakeresult);

        char *Hexstring_temp = new char[BUF_SIZE];
        memset(Hexstring_temp, 0, BUF_SIZE * sizeof(char));

        uint8_t *Hexstring_uint8_temp = new uint8_t[BUF_SIZE];
        memset(Hexstring_uint8_temp, 0, BUF_SIZE * sizeof(uint8_t)); //Ŀǰ��������ؼ��Ķ�̬���飬�����̬�ͻ��ջ����

        uint16_t payload_crc16_calc_temp = 0;

        Bin2Hex(fakeresult, Hexstring_temp);

        Char2Uint(Hexstring_temp, Hexstring_uint8_temp);
        delete[] Hexstring_temp;

        payload_crc16_calc_temp = sx1302_lora_payload_crc(Hexstring_uint8_temp, length);
        delete[] Hexstring_uint8_temp;

        if (payload_crc16_calc_temp == crc_int) {
            strcpy(realresult, fakeresult);
            pass_crc++;
        }

        /*测试代码
        total_number++;
        printf("The number of candidate: %d\n", total_number);
        */

    } else {
        num[m] = '0';
        Search(input, m + 1, mch, crc_int, fakeresult, realresult, length, pass_crc, total_number, startTime);
        num[m] = '1';
        Search(input, m + 1, mch, crc_int, fakeresult, realresult, length, pass_crc, total_number, startTime);
    }
}


void
correct(char *input, char *mch, int Hamming_weight_now, int crc_int, char *fakeresult, char *realresult, int length,
        int &pass_crc, int &total_number, struct timeval startTime) {

    n = Hamming_weight_now;
    int m = 0;
    Search(input, m, mch, crc_int, fakeresult, realresult, length, pass_crc, total_number, startTime);

}

/* -------------------------------------------------------------------------- */
/* --- Incremental correct ---------------------- */

void dfs(vector<vector<int>> &res, vector<int> &output, int pos, int len, bool bflag) {
    if (len == int(output.size()) - 1) {
        res.push_back(output);
        return;
    }
    for (int i = pos; i < int(output.size()); ++i) {
        if (i > 0 && output[i - 1] == output[i] && bflag)
            continue;
        bflag = true;
        swap(output[i], output[pos]);
        sort(output.begin() + pos + 1, output.end());
        dfs(res, output, pos + 1, len + 1, false);
        swap(output[i], output[pos]);
    }
}

vector<vector<int>> qpl(vector<int> &nums) {
    vector <vector<int>> res;
    int l = nums.size();
    dfs(res, nums, 0, 0, false);
    return res;
}

void output(int n, char *input, char *mch, int crc_int, char *fakeresult, char *realresult, int length, int &pass_crc,
            int &total_number, struct timeval startTime) {

    struct timeval nowTime;
    gettimeofday(&nowTime, NULL);
    double timeuse = (nowTime.tv_sec - startTime.tv_sec) + (double) (nowTime.tv_usec - startTime.tv_usec) / 1000000.0;
    if (timeuse >= 5) {
        printf("Time exceed!\n");
        return;
    }

    vector <vector<int>> x;
    for (int i = 0; i <= n; i++) {
        vector<int> nums(n);
        for (int j = 0; j < i; j++)
            nums[j] = 1;
        sort(nums.begin(), nums.end());
        auto ans = qpl(nums);
        for (auto &v: ans)
            x.push_back(v);
    }

    vector <string> ans;
    for (auto &v: x) {
        stringstream st;
        for (auto &n: v) {
            st << n;
        }
        string str;
        st >> str;
        ans.push_back(str);

        char str_char[BUF_SIZE];
        strcpy(str_char, str.c_str());


        for (int j = 0; j <= strlen(input) - 1; j++) {
            if (input[j] == '0') {
                insertzero(str_char, j);
            }
        }

        //cout<<str_char<<endl;

        OZ_bin_xor(mch, str_char, fakeresult);
        //printf("%s\n", fakeresult);

        char *Hexstring_temp = new char[BUF_SIZE];
        memset(Hexstring_temp, 0, BUF_SIZE * sizeof(char));

        uint8_t *Hexstring_uint8_temp = new uint8_t[BUF_SIZE];
        memset(Hexstring_uint8_temp, 0, BUF_SIZE * sizeof(uint8_t)); //Ŀǰ��������ؼ��Ķ�̬���飬�����̬�ͻ��ջ����

        uint16_t payload_crc16_calc_temp = 0;

        Bin2Hex(fakeresult, Hexstring_temp);

        Char2Uint(Hexstring_temp, Hexstring_uint8_temp);
        delete[] Hexstring_temp;

        payload_crc16_calc_temp = sx1302_lora_payload_crc(Hexstring_uint8_temp, length);
        delete[] Hexstring_uint8_temp;

        if (payload_crc16_calc_temp == crc_int) {
            strcpy(realresult, fakeresult);
            pass_crc++;
        }

        if (pass_crc == 1) {

            return; //pass_crc=1说明已经有一个crc校验通过的了，直接退出，这样会直接根除掉假阳性false positives (Hash碰撞)

        }

        /*测试代码
        total_number++;
        printf("The number of candidate: %d\n", total_number);
        */

    }

}

void
incremental_correct(char *input, char *mch, int Hamming_weight_now, int crc_int, char *fakeresult, char *realresult,
                    int length, int &pass_crc, int &total_number, struct timeval startTime) {

    output(Hamming_weight_now, input, mch, crc_int, fakeresult, realresult, length, pass_crc, total_number, startTime);
}