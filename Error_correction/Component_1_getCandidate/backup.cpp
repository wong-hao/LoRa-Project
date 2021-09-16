#include "backup.h"

using namespace std;

int countone(char* input){

    int count = 0;

    for (int i = 0; i < strlen(input); i++) {
        if (input[i] == '1') count++;
    }

    return count;

}

void insertzero(char* input, int location){

    std::string input_str(input);
    input_str.insert(location,"0");
    strcpy(input,input_str.c_str());

}

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

void incremental_correct(char* input) {

    output(countone(input), input);
}

int main() {
    char x[20];
    cin >> x;
    incremental_correct(x);

    return 0;
}