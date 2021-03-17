#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<algorithm>
#include <queue>

using namespace std;

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

vector<fstream*> getFileHandlers(string fileName,int noOfSublist){
    vector<fstream*> ans;
    for(int i=0;i<noOfSublist;i++){
        string sublistFileName = fileName +"_bucket_no_"+to_string(i);
        fstream* f= new fstream(sublistFileName,fstream::in|fstream::out|fstream::app);
        ans.push_back(f);
    }
    return ans;
}

vector<string> splitString(string s){
    stringstream ss(s);
    vector<string> ans;
    string line;
    while(getline(ss,line,' ')){
        ans.push_back(line);
    }
    return ans;
}

vector<vector<string>> readBlock(ifstream* fileHandler,int numOfTuplesInBlock){
    //cout<<"read block is called "<<endl;
    vector<vector<string>> ans;
    if(!fileHandler->is_open()){
        //cout<<"entered fileHandler is closed condition "<<endl;
        return ans;
    }
    else{
        string currStr;
        while(getline((*fileHandler),currStr)){
            //cout<<"entered the while loop "<<endl;
            //cout<<currStr<<endl;
            vector<string> tuple = splitString(currStr);
            ans.push_back(tuple);
            if(ans.size() == numOfTuplesInBlock){
                // need to sort listOfTuples then write it bac
                return ans;
            }
        }
        //cout<<"Exitied while loop "<<endl;
        fileHandler->close();
        return ans;
    } 
}

void writeBlock(vector<vector<string>>& listOfTuples,fstream* myfile){
    for(auto vecS:listOfTuples){
        string currString = "";
        for(auto s:vecS){
            currString += (s+' ');
        }
        *myfile<<currString<<endl;
    }
}

void makeSublist(string fileNameR,int numberOfBuckets,int numOfTuplesInBlock){
    vector<fstream*> fileHandlersR = getFileHandlers(fileNameR,numberOfBuckets);
    vector<vector<vector<string>>> bucketsOfR(numberOfBuckets);
    ifstream Rfile(fileNameR);
    while(Rfile.is_open()){
        vector<vector<string>> currBlock = readBlock(&Rfile,numOfTuplesInBlock);
        for(auto tuple : currBlock){
            int hashVal = hashFunc(tuple[1],numberOfBuckets);
            if(bucketsOfR[hashVal].size()<numOfTuplesInBlock){
                bucketsOfR[hashVal].push_back(tuple);
            }
            else{
                writeBlock(bucketsOfR[hashVal],fileHandlersR[hashVal]);
                bucketsOfR[hashVal].clear();
                bucketsOfR[hashVal].push_back(tuple);
            }
        }
    }
    for(int i=0;i<bucketsOfR.size();i++){
        if(bucketsOfR[i].size()!=0){
            writeBlock(bucketsOfR[i],fileHandlersR[i]);
        }
        bucketsOfR[i].clear();
    }
    for(auto fh:fileHandlersR){
        fh->close();
    }
}

int main(){
    int numOfTuplesInBlock = 4; //given in assignment
    int M = 10; // number of blocks in main memory
    int numOfTuplesInSublist = M*numOfTuplesInBlock;
    int numberOfBuckets = M-1;

    string fileNameR = "inputR";
    string fileNameS = "inputS";
    string outputFileName = fileNameR+"_"+fileNameS+"_join.txt";
    ofstream outputHandler(outputFileName);

    //need to read one block from the file. Assign the bucket for each tuple in the block
    // hash values will range from 0 to m-2
    makeSublist(fileNameR,numberOfBuckets,numOfTuplesInBlock);
    makeSublist(fileNameS,numberOfBuckets,numOfTuplesInBlock);
}