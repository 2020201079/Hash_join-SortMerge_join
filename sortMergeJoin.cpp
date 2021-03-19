#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<algorithm>
#include <queue>
#include <chrono> 

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
    //cout<<"file name is "<<inputFile<<endl;
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
    Rfile.close();

    if(listOfTuples.size() >0){
        sortAndWriteBack(listOfTuples,inputFile,numOfSublist,sortIndex);
    }
    else{
        numOfSublist--;
    }
    return numOfSublist;
}

vector<ifstream*> getFileHandlers(string fileName,int noOfSublist){
    vector<ifstream*> ans;
    for(int i=1;i<=noOfSublist;i++){
        string sublistFileName = fileName + to_string(i);
        ifstream* f= new ifstream(sublistFileName);
        ans.push_back(f);
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

struct CompareR {
    bool operator()(pair<vector<string>,int> const& p1, pair<vector<string>,int> const& p2)
    {
        return p1.first[1] > p2.first[1];
    }
};

struct CompareS {
    bool operator()(pair<vector<string>,int> const& p1, pair<vector<string>,int> const& p2)
    {
        return p1.first[0] > p2.first[0];
    }
};

void print_pq(priority_queue<pair<vector<string>,int>,vector<pair<vector<string>,int>>,CompareR> pq_R){
    // this function is just for testing
    while(!pq_R.empty()){
        pair<vector<string>,int> top = pq_R.top();
        pq_R.pop();
        cout<<top.first[0]<<" "<<top.first[1]<<" "<<top.second<<endl;
    }
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

    string fileNameR = argv[1];
    string fileNameS = argv[2];
    string outputFileName = fileNameR+"_"+fileNameS+"_join.txt";
    ofstream outputHandler(outputFileName);
    
    // for part1 we need to read M blocks from R and S
    // sort those and save in sublist
    int noOfSublistR =  makeSortedSublist(numOfTuplesInSublist,fileNameR,1);
    int noOfSublistS =  makeSortedSublist(numOfTuplesInSublist,fileNameS,0);

    cout<<"no of sublist R : "<<noOfSublistR<<endl;
    cout<<"no of sublist S : "<<noOfSublistS<<endl;

    if(noOfSublistR + noOfSublistS > M){
        cout<<"cannot fit in memory check B(R) + B(S) < M^2"<<endl;
    }

    //now read each block from a sublist

    vector<ifstream*> fileHandlersR = getFileHandlers(fileNameR,noOfSublistR);
    vector<vector<vector<string>>> blocksFromR;
    for(int i=0;i<noOfSublistR;i++){
        blocksFromR.push_back(readBlock(fileHandlersR[i],numOfTuplesInBlock));
    }

    vector<ifstream*> fileHandlersS = getFileHandlers(fileNameS,noOfSublistS);
    vector<vector<vector<string>>> blocksFromS;
    for(int i=0;i<noOfSublistS;i++){
        blocksFromS.push_back(readBlock(fileHandlersS[i],numOfTuplesInBlock));
    }
    
    // put the first element from each block in a priority queue
    priority_queue<pair<vector<string>,int>,vector<pair<vector<string>,int>>,CompareR> pq_R;
    for(int i=0;i<noOfSublistR;i++){
        pq_R.push({blocksFromR[i][0],i});
        blocksFromR[i].erase(blocksFromR[i].begin());
    }

    priority_queue<pair<vector<string>,int>,vector<pair<vector<string>,int>>,CompareS> pq_S;
    for(int i=0;i<noOfSublistS;i++){
        pq_S.push({blocksFromS[i][0],i});
        blocksFromS[i].erase(blocksFromS[i].begin());
    }

    while(!pq_S.empty() && !pq_R.empty()){
        auto topR = pq_R.top();
        auto topS = pq_S.top();
        vector<string> tupleR = topR.first;
        vector<string> tupleS = topS.first;
        int sublistNoR = topR.second;
        int sublistNoS = topS.second;
        
        if(tupleR[1] == tupleS[0]){
            vector<vector<string>> joinR;
            vector<vector<string>> joinS;
            string commonY = tupleR[1];
            //cout<<"common Y is : "<<commonY<<endl;
            while(!pq_R.empty()){
                auto currTuple = pq_R.top().first;
                auto currSublist = pq_R.top().second;
                if(currTuple[1] == commonY){
                    joinR.push_back(currTuple);
                    pq_R.pop(); // pop
                    // need to push an element from the sublist block
                    if(blocksFromR[currSublist].size() == 0 && fileHandlersR[currSublist]->is_open()){
                        blocksFromR[currSublist] = readBlock(fileHandlersR[currSublist],numOfTuplesInBlock);
                    }
                    if(blocksFromR[currSublist].size() != 0){
                        pq_R.push({blocksFromR[currSublist][0],currSublist});
                        blocksFromR[currSublist].erase(blocksFromR[currSublist].begin());
                    }
                }
                else{
                    break;
                }
            }

            while(!pq_S.empty()){
                auto currTuple = pq_S.top().first;
                auto currSublist = pq_S.top().second;
                if(currTuple[0] == commonY){
                    joinS.push_back(currTuple);
                    pq_S.pop(); // pop
                    // need to push an element from the sublist block
                    if(blocksFromS[currSublist].size() == 0 && fileHandlersS[currSublist]->is_open()){
                        blocksFromS[currSublist] = readBlock(fileHandlersS[currSublist],numOfTuplesInBlock);
                    }
                    if(blocksFromS[currSublist].size() != 0){
                        pq_S.push({blocksFromS[currSublist][0],currSublist});
                        blocksFromS[currSublist].erase(blocksFromS[currSublist].begin());
                    }
                }
                else{
                    break;
                }
            }
            //cout<<"List for joining is as follows : "<<endl;
            for(auto vecR : joinR ){
                for(auto vecS : joinS){
                    string outputString = vecR[0] + " "+ vecR[1]+" "+vecS[1];
                    //cout<<outputString<<endl;
                    outputHandler<<outputString<<endl;
                }
            }
            joinR.clear();
            joinS.clear();
        }
        else if(tupleR[1] < tupleS[0]){
            //pop out pq_R
            //auto currTuple = pq_R.top().first;
            auto currSublist = pq_R.top().second;
            pq_R.pop(); // pop
            // need to push an element from the sublist block
            if(blocksFromR[currSublist].size() == 0 && fileHandlersR[currSublist]->is_open()){
                blocksFromR[currSublist] = readBlock(fileHandlersR[currSublist],numOfTuplesInBlock);
            }
            if(blocksFromR[currSublist].size() != 0){
                pq_R.push({blocksFromR[currSublist][0],currSublist});
                blocksFromR[currSublist].erase(blocksFromR[currSublist].begin());
            }
        }

        else{
            //pop out pq_S
            //auto currTuple = pq_S.top().first;
            auto currSublist = pq_S.top().second;
            pq_S.pop(); // pop
            // need to push an element from the sublist block
            if(blocksFromS[currSublist].size() == 0 && fileHandlersS[currSublist]->is_open()){
                blocksFromS[currSublist] = readBlock(fileHandlersS[currSublist],numOfTuplesInBlock);
            }
            if(blocksFromS[currSublist].size() != 0){
                pq_S.push({blocksFromS[currSublist][0],currSublist});
                blocksFromS[currSublist].erase(blocksFromS[currSublist].begin());
            }
        }
    }
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start); 
    cout <<"M : "<<M<<" time to sort in microseconds " <<duration.count() << endl; 
}