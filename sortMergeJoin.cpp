#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<algorithm>

using namespace std;

bool sortcol1( const vector<string>& v1,const vector<string>& v2 ) { 
    return v1[1] < v2[1]; 
}

bool sortcol0( const vector<string>& v1,const vector<string>& v2 ) { 
    return v1[0] < v2[0]; 
}

void sortVectOfStrings(vector<vector<string>>& listOfTuples,int sortIndex){
    if(sortIndex == 1){
        sort(listOfTuples.begin(),listOfTuples.end(),sortcol1);
    }
    else if(sortIndex == 0){
        sort(listOfTuples.begin(),listOfTuples.end(),sortcol0);
    }
    else{
        cout<<"error sort on 0 or 1 only"<<endl;
    }
}

void sortAndWriteBack(vector<vector<string>>& listOfTuples,string relName,int numOfSublist,int sortIndex){
    sortVectOfStrings(listOfTuples,sortIndex);
    string fileName = relName + to_string(numOfSublist);
    ofstream myfile(fileName);
    if( myfile.fail() )
    {
        cout<<"error openning file "<<endl;
        cerr << "!Error opening " << fileName << endl;
    }
    for(auto vecS:listOfTuples){
        string currString = "";
        for(auto s:vecS){
            currString += (s+' ');
        }
        myfile<<currString<<endl;
    }
    myfile.close();
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

int makeSortedSublist(int numOfTuplesInSublist,string inputFile,int sortIndex){
    ifstream Rfile(inputFile);
    vector<vector<string>> listOfTuples; // tuple is a vector of string
    string currStr;

    int numOfSublist = 1; // lets start from 1
    while(getline(Rfile,currStr)){
        vector<string> tuple = splitString(currStr);
        listOfTuples.push_back(tuple);
        if(listOfTuples.size() == numOfTuplesInSublist){
            // need to sort listOfTuples then write it back
            sortAndWriteBack(listOfTuples,inputFile,numOfSublist,sortIndex);
            numOfSublist++;
            listOfTuples.clear();
        }
    }

    if(listOfTuples.size() >0){
        sortAndWriteBack(listOfTuples,inputFile,numOfSublist,1);
    }
    else{
        numOfSublist--;
    }
    return numOfSublist;
}

int main(){
    int numOfTuplesInBlock = 3; //given in assignment
    int M = 2; // number of blocks in main memory
    int numOfTuplesInSublist = M*numOfTuplesInBlock;

    string fileNameR = "inputR";
    string fileNameS = "inputS";
    
    // for part1 we need to read M blocks from R and S
    // sort those and save in sublist
    int noOfSublistR =  makeSortedSublist(numOfTuplesInSublist,fileNameR,1);
    int noOfSublistS =  makeSortedSublist(numOfTuplesInSublist,fileNameS,0);

    cout<<"no of sublist R : "<<noOfSublistR<<endl;
    cout<<"no of sublist S : "<<noOfSublistS<<endl;
}