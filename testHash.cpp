#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<algorithm>
#include <queue>

using namespace std;

vector<string> splitString(string s){
    stringstream ss(s);
    vector<string> ans;
    string line;
    while(getline(ss,line,' ')){
        ans.push_back(line);
    }
    return ans;
}

int hashFunc(string str,int m) // here M-1 should be passed
{
    int p = 31;
    long long power_of_p = 1;
    long long hash_val = 0;
 
    // Loop to calculate the hash value
    // by iterating over the elements of string
    for (int i = 0; i < str.length(); i++) {
        hash_val= (hash_val+ (str[i] - 'a' + 1) * power_of_p)% m;
        power_of_p = (power_of_p * p) % m;
    }
    return hash_val;
}

void printHashValues(string fname){
    auto f = ifstream(fname);
    string currStr;
    while(getline(f,currStr)){
        vector<string> tuple = splitString(currStr);
        cout<<hashFunc(tuple[1],9)<<endl;
    }
}


int main(){
    string sublistFileName = "inputR_bucket_no_"+to_string(8);
    printHashValues(sublistFileName);
}