//https://blog.csdn.net/zhao2018/article/details/82803070?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522163133865416780264014767%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=163133865416780264014767&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~rank_v29_ecpm-1-82803070.pc_search_result_cache&utm_term=C%2B%2B%E7%A9%B7%E4%B8%BEn%E4%BD%8D%E4%BA%8C%E8%BF%9B%E5%88%B6%E6%95%B0&spm=1018.2226.3001.4187
//https://blog.csdn.net/sayniceoh/article/details/53105466?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522163133865416780264014767%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=163133865416780264014767&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~rank_v29_ecpm-3-53105466.pc_search_result_cache&utm_term=C%2B%2B%E7%A9%B7%E4%B8%BEn%E4%BD%8D%E4%BA%8C%E8%BF%9B%E5%88%B6%E6%95%B0&spm=1018.2226.3001.4187
//https://www.codeleading.com/article/3956123395/
//https://blog.csdn.net/acsuperman/article/details/79930557
//https://www.geek-share.com/detail/2720457588.html

#include "header_1_2.h"

/* -------------------------------------------------------------------------- */
/* --- Correct ---------------------- */

char num[20];
char num2[20];
int n;


void insertzero(char* input, int location){

    std::string input_str(input);
    input_str.insert(location,"0");
    strcpy(input,input_str.c_str());

}

void Search(char* input, int m)
{
    int i;
    if(m == n)
    {
        //printf("%s",num);

        strcpy(num2,num);

        for(int j=0;j<= strlen(input)-1;j++){
            if(input[j]=='0'){
                insertzero(num2,j);
            }
        }

        printf("%s\n",num2);
    }
    else
    {
        num[m]='0';Search(input, m+1);
        num[m]='1';Search(input, m+1);
    }
}


void correct(char* input, int Hamming_weight_now) {

    n = Hamming_weight_now;
    int m = 0;
    Search(input, m);
}

/* -------------------------------------------------------------------------- */
/* --- Incremental correct ---------------------- */

void dfs(vector<vector<int>>& res, vector<int>& output, int pos, int len, bool bflag) {
    if (len == int(output.size()) - 1)
    {
        res.push_back(output);
        return;
    }
    for (int i = pos; i<int(output.size()); ++i) {
        if (i > 0 && output[i - 1] == output[i]&&bflag)
            continue;
        bflag = true;
        swap(output[i], output[pos]);
        sort(output.begin() + pos+1, output.end());
        dfs(res, output, pos + 1, len + 1,false);
        swap(output[i], output[pos]);
    }
}
vector<vector<int>> qpl(vector<int>& nums) {
    vector<vector<int>> res;
    int l = nums.size();
    dfs(res, nums, 0, 0,false);
    return res;
}

void output(int n, char* input){
    vector<vector<int>> x;
    for (int i = 0; i <= n; i++) {
        vector<int> nums(n);
        for (int j = 0; j < i; j++)
            nums[j] = 1;
        sort(nums.begin(), nums.end());
        auto ans = qpl(nums);
        for (auto& v : ans)
            x.push_back(v);
    }

    vector<string> ans;
    for (auto& v : x) {
        stringstream st;
        for (auto& n : v) {
            st << n;
        }
        string str;
        st >> str;
        ans.push_back(str);

        char str_char[BUF_SIZE];
        strcpy(str_char,str.c_str());


        for(int j=0;j<= strlen(input)-1;j++){
            if(input[j]=='0'){
                insertzero(str_char,j);
            }
        }

        cout<<str_char<<endl;

    }

}

void incremental_correct(char* input, int Hamming_weight_now) {

    output(Hamming_weight_now, input);
}

int main()
{

    char Binarystring3[20] = "01111";

    int Hamming_weight_max = 30;
    int Hamming_weight_now = 4;

    if(Hamming_weight_now <= Hamming_weight_max/2){
        incremental_correct(Binarystring3, Hamming_weight_now);
    }else{
        correct(Binarystring3,Hamming_weight_now);
    }

    return 0;
}
