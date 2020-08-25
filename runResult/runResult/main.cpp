//
//  main.cpp
//  runResult
//
//  Created by bobobo on 24/08/2020.
//  Copyright © 2020 bobobo. All rights reserved.
//

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>
using namespace std;

void readFile(string filePath, map<int, int> &branch2Feature)
{
    string eachLine;
    ifstream resFile;
    resFile.open(filePath);
    while (getline(resFile, eachLine)) {
        vector<string> tokens;
        istringstream iss(eachLine);
        copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(tokens));
        cout << "Branch " << tokens[0] << " -> Feature " << tokens[2] << endl;
        branch2Feature.insert(pair<int, int>(stoi(tokens[0]), stoi(tokens[2])-1));
    }
    resFile.close();
}

void genDataTable(string dataPath, vector<vector<int>> &dataTable)
{
    string eachLine;
    ifstream dataFile;
    dataFile.open(dataPath);
    while (getline(dataFile, eachLine)) {
        cout << eachLine << endl;
        vector<int> tokens;
        stringstream ss(eachLine);
        for (int i; ss >> i; ) {
            tokens.push_back(i);
            if(ss.peek() == ',')
                ss.ignore();
        }
        dataTable.push_back(tokens);
    }
}

void printDataTable(vector<vector<int>> &dataTable)
{
    for(vector<vector<int>>::iterator it = dataTable.begin(); it < dataTable.end(); it++)
    {
        vector<int> dataItem = *it;
        for (vector<int>::iterator vit = dataItem.begin(); vit < dataItem.end(); vit++) {
            cout << *vit << " ";
        }
        cout << endl;
    }
}

void gatherLeaves(map<int, int> &branch2Feature, vector<int> &leaves)
{
    // set the map value of leaves as "0", which means that there is no matching feature
    for(map<int,int>::iterator it = branch2Feature.begin(); it != branch2Feature.end(); it++)
    {
        if((branch2Feature.find((it->first)*2) != branch2Feature.end()) && (branch2Feature.find((it->first)*2 + 1) != branch2Feature.end()))
            continue;
        if (branch2Feature.find((it->first)*2) == branch2Feature.end())
        {
            leaves.push_back((it->first)*2);
        }
        if (branch2Feature.find((it->first)*2+1) == branch2Feature.end())
        {
            leaves.push_back((it->first)*2+1);
        }
            
    }
}

void runClass(vector<vector<int>> &dataTable, map<int, int> &branch2Feature, vector<int> &leaves, map<int, int> &leaveClass)
{
    map<int, int> index2leave;
    vector<int> leaveCount;
    int cnt = 1;
    for(vector<int>::iterator it = leaves.begin(); it != leaves.end(); it++){
        index2leave.insert(pair<int, int>(*it, cnt));
        cnt++;
        leaveCount.push_back(0);
    }
    for (vector<vector<int>>::iterator vvit = dataTable.begin(); vvit != dataTable.end(); ++vvit) {
        int branchNow = 1;
        while (branch2Feature.find(branchNow) != branch2Feature.end()) {
            if(*vvit[branch2Feature[branchNow]] == 0)
            {
                // left branch
                branchNow = branchNow*2;
            }else{
                // right branch
                branchNow = branchNow*2+1;
            }
        }
        int len = (*vvit).size();
        if((*vvit)[len-1] == 1)
            leaveCount[index2leave[branchNow]] += 1;
        else
            leaveCount[index2leave[branchNow]] += -1;
    }
    
    for(map<int, int>::iterator it = index2leave.begin(); it != index2leave.end(); it++)
    {
        int res = leaveCount[it->second];
        leaveClass.insert(pair<int, int>(it->first, res));
    }
}

int calError(map<int, int> &branch2Feature, map<int, int> &leaveClass, vector<vector<int>> &dataTable, vector<int> &Loss)
{
    map<int, int> index2leave;
    int cnt = 1;
    for(vector<int>::iterator it = leaves.begin(); it != leaves.end(); it++){
        index2leave.insert(pair<int, int>(*it, cnt));
        cnt++;
        Loss.push_back(0);
    }
    for (vector<vector<int>>::iterator vvit = dataTable.begin(); vvit != dataTable.end(); ++vvit){
        int branchNow = 1;
        while (branch2Feature.find(branchNow) != branch2Feature.end()) {
            if(*vvit[branch2Feature[branchNow]] == 0)
            {
                // left branch
                branchNow = branchNow*2;
            }else{
                // right branch
                branchNow = branchNow*2+1;
            }
        }
        int len = (*vvit).size();
        if((*vvit)[len-1] != leaveClass[branchNow])
            Loss[index2leave[branchNow]] += 1;
    }
    // print error for each leave node
    int sumErr = 0;
    for (map<int, int>::iterator it = index2leave.begin(); it != index2leave.end(); it++) {
        cout << "leave node [" << it->first << "], error is " << Loss[it->second];
        sumErr += Loss[it->second];
    }
    return sumErr;
}

int main(int argc, const char * argv[]) {
    // argv[1] : the file path of the encoding result
    // argv[2] : the dataset represented in the CSV file
    map<int, int> branch2Feature;
    readFile(argv[1], branch2Feature);
    vector<vector<int>> dataTable;
    genDataTable(argv[2], dataTable);
    vector<int> leaves;
    gatherLeaves(branch2Feature, leaves);
    //printDataTable(dataTable);
    map<int, int> leaveClass;
    runClass(dataTable, branch2Feature, leaves, leaveClass);
    vector<int> Loss;
    int error = calError(branch2Feature, leaveClass, dataTable, Loss);
    return 0;
}
