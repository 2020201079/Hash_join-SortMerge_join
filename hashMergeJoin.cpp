#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<algorithm>
#include <queue>
#include <stdio.h>
#include <chrono> 

using namespace std;

string getFileName(const string& s) {

   char sep = '/';

   size_t i = s.rfind(sep, s.length());
   if (i != string::npos) {
      return(s.substr(i+1, s.length() - i));
   }

   return("");
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

vector<int> makeSublist(string fileNameR,int numberOfBuckets,int numOfTuplesInBlock,int commonIndex){
    vector<int> countTuplesInBucket(numberOfBuckets,0);
    vector<fstream*> fileHandlersR = getFileHandlers(fileNameR,numberOfBuckets);
    vector<vector<vector<string>>> bucketsOfR(numberOfBuckets);
    ifstream Rfile(fileNameR);
    while(Rfile.is_open()){
        vector<vector<string>> currBlock = readBlock(&Rfile,numOfTuplesInBlock);
        for(auto tuple : currBlock){
            int hashVal = hashFunc(tuple[commonIndex],numberOfBuckets);
            if(bucketsOfR[hashVal].size()<numOfTuplesInBlock){
                bucketsOfR[hashVal].push_back(tuple);
            }
            else{
                countTuplesInBucket[hashVal] += bucketsOfR[hashVal].size();
                writeBlock(bucketsOfR[hashVal],fileHandlersR[hashVal]);
                bucketsOfR[hashVal].clear();
                bucketsOfR[hashVal].push_back(tuple);
            }
        }
    }
    for(int i=0;i<bucketsOfR.size();i++){
        if(bucketsOfR[i].size()!=0){
            countTuplesInBucket[i] += bucketsOfR[i].size();
            writeBlock(bucketsOfR[i],fileHandlersR[i]);
            bucketsOfR[i].clear();
        }
    }
    for(auto fh:fileHandlersR){
        fh->close();
    }
    return countTuplesInBucket;
}

int main(int argc, char** argv){
    if(argc != 4){
        cout<<"Input arguments are wrong the format is "<<endl;
        cout<<"a.out inputR inputS M"<<endl;
        return 0;
    }
    auto start = chrono::high_resolution_clock::now();
    int numOfTuplesInBlock = 100; //given in assignment
    int M = stoi(argv[3]); // number of blocks in main memory
    int numOfTuplesInSublist = M*numOfTuplesInBlock;
    int numberOfBuckets = M-1;


    string fileNameR = argv[1];
    string fileNameS = argv[2];
    string outputFileName = getFileName(fileNameR)+"_"+getFileName(fileNameS)+"_join.txt";
    ofstream outputHandler(outputFileName);

    //need to read one block from the file. Assign the bucket for each tuple in the block
    // hash values will range from 0 to m-2
    vector<int> countTuplesInBucketR = makeSublist(fileNameR,numberOfBuckets,numOfTuplesInBlock,1);
    vector<int> countTuplesInBucketS = makeSublist(fileNameS,numberOfBuckets,numOfTuplesInBlock,0);

    for(int i=0;i<countTuplesInBucketR.size();i++){
        cout<<"Number of tuples in bucket "<<i<<" for R is "<<countTuplesInBucketR[i]<<endl;
    }

    for(int i=0;i<countTuplesInBucketS.size();i++){
        cout<<"Number of tuples in bucket "<<i<<" for S is "<<countTuplesInBucketS[i]<<endl;
    }

    //after phase 1 now need to merge the hashed buckets
    for(int i=0;i<numberOfBuckets;i++){
        cout<<"merging bucket number "<<i<<endl;
        string fileNameMax;
        string fileNameMin;
        int minTupleCount;
        int maxTupleCount;
        if(countTuplesInBucketR[i] >= countTuplesInBucketS[i]){
            fileNameMax = fileNameR;
            maxTupleCount = countTuplesInBucketR[i];
            fileNameMin = fileNameS;
            minTupleCount = countTuplesInBucketS[i];
        }
        else{
            fileNameMax = fileNameS;
            maxTupleCount = countTuplesInBucketS[i];
            fileNameMin = fileNameR;
            minTupleCount = countTuplesInBucketR[i];
        }
        if(minTupleCount > (M-1)*numOfTuplesInBlock){
            cout<<"Bucket number "<<i<<" cannot be merged because the smaller bucket does not fit in memory "<<endl; 
            return -1;
        }

        //read the smaller file name in the memory
        string sublistFileNameSmaller = fileNameMin +"_bucket_no_"+to_string(i);
        string sublistFileNameLarger = fileNameMax +"_bucket_no_"+to_string(i);
        ifstream fmin(sublistFileNameSmaller);
        ifstream fmax(sublistFileNameLarger);
        string currString;
        vector<vector<string>> smallerRel;
        while(getline(fmin,currString)){
            smallerRel.push_back(splitString(currString));
        }
        fmin.close();
        while(fmax.is_open()){
            vector<vector<string>> currBlock = readBlock(&fmax,numOfTuplesInBlock);
            for(auto tupleMax:currBlock){
                for(auto tupleMin:smallerRel) {
                    if(fileNameMax == fileNameR){ //R is max S is Min
                        if(tupleMax[1] == tupleMin[0]){
                            string outputString = tupleMax[0]+" "+tupleMax[1]+" "+tupleMin[1];
                            outputHandler<<outputString<<endl;
                        }
                    }
                    else{ //R is min S is max
                        if(tupleMax[0] == tupleMin[1]){
                            string outputString = tupleMin[0]+" "+tupleMin[1]+" "+tupleMax[1];
                            outputHandler<<outputString<<endl;
                        }
                    }
                }
            } 
        }
        fmax.close();
    }
    outputHandler.close();
    cout<<"finished joining "<<endl;

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start); 
    cout <<"M : "<<M<<" time to sort in microseconds " <<duration.count() << endl; 

    cout<<"deleting intermediate files "<<endl;
    for(int i=0;i<numberOfBuckets;i++){
        string deleteR = fileNameR +"_bucket_no_"+to_string(i);
        string deleteS = fileNameS +"_bucket_no_"+to_string(i);
        remove(deleteR.c_str());
        remove(deleteS.c_str());
    }

}