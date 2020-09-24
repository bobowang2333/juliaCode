//
//  main.cpp
//  runResult
//
//  Created by bobobo on 24/08/2020.
//  Copyright © 2020 bobobo. All rights reserved.
//

#include <string>
#include <tuple>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>
using namespace std;
typedef tuple<int, int, int, int> sensitiveInfo;

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
    int cnt = 1;
    while (getline(dataFile, eachLine)) {
        cout << eachLine << endl;
        if (cnt == 1)
        {
            // avoid the dataTable takes the header as one line
            cnt += 1;
            continue;
        }
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
            if((*vvit)[(branch2Feature.find(branchNow))->second] == 0)
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
        if(res >= 0)
            leaveClass.insert(pair<int, int>(it->first, 1));
        else
            leaveClass.insert(pair<int, int>(it->first, 0));
    }
}

void printClass(map<int, int> &leaveClass)
{
    for(map<int, int>::iterator it = leaveClass.begin(); it != leaveClass.end(); it++)
    {
        cout << "leave ID [" << it->first << "], output class [" << it->second << "]" << endl;
    }
}

int calError(map<int, int> &branch2Feature, vector<int> &leaves, map<int, int> &leaveClass, vector<vector<int>> &dataTable, vector<int> &Loss)
{
    map<int, int> index2leave;
    vector<vector<int>> leave2Example;
    int cnt = 0;
    for(vector<int>::iterator it = leaves.begin(); it != leaves.end(); it++){
        index2leave.insert(pair<int, int>(*it, cnt));
        cnt++;
        Loss.push_back(0);
        
        vector<int> emptyVec;
        leave2Example.push_back(emptyVec);
    }
    int exampleID = 1;
    for (vector<vector<int>>::iterator vvit = dataTable.begin(); vvit != dataTable.end(); ++vvit){
        int branchNow = 1;
        while (branch2Feature.find(branchNow) != branch2Feature.end()) {
            if((*vvit)[(branch2Feature.find(branchNow))->second] == 0)
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
        
        // add the structure associate the leaves with the set of examples
        leave2Example[index2leave[branchNow]].push_back(exampleID);
        exampleID++;
    }
    // print error for each leave node
    int sumErr = 0;
    for (map<int, int>::iterator it = index2leave.begin(); it != index2leave.end(); it++) {
        cout << "leave node [" << it->first << "], error is " << Loss[it->second] << endl;
        sumErr += Loss[it->second];
    }
    
    // print leaves and the associated examples
    cnt = 0;
    for(vector<int>::iterator it = leaves.begin(); it != leaves.end(); it++){
        cout << "leave ID [" << *it << "] -> Examples [";
        for(vector<int>::iterator lit = leave2Example[index2leave[*it]].begin(); lit != leave2Example[index2leave[*it]].end(); lit++)
        {
            cout << *lit << " ";
        }
        cout << "]" << endl;
    }
    return sumErr;
}

void calLeaf2Percentage(vector<vector<int>> &dataTable, map<int, int> &branch2Feature, map<int, int> &leaveClass, map<int, sensitiveInfo> &leaf2Percentage, int senID)
{
    map<int, int> leaf2index;
    
    int count = 0;
    for (map<int, int>::iterator it = leaveClass.begin(); it != leaveClass.end(); it++) {
        leaf2index.insert(pair<int,int>(it->first, count));
        count++;
    }
    
    vector<vector<int>> leaf2Examples;
    // initialize the leaf2Examples vector
    int numLeaves = leaveClass.size();
    for (int i = 0; i < numLeaves; ++i) {
        vector<int> emptyVec;
        leaf2Examples.push_back(emptyVec);
    }
    
    // In dataTable, the first data row starts from 0
    int cnt = 0;
    for (vector<vector<int>>::iterator vvit = dataTable.begin(); vvit != dataTable.end(); ++vvit){
        int branchNow = 1;
        while (branch2Feature.find(branchNow) != branch2Feature.end()) {
            if((*vvit)[(branch2Feature.find(branchNow))->second] == 0)
            {
                // left branch
                branchNow = branchNow*2;
            }else{
                // right branch
                branchNow = branchNow*2+1;
            }
        }
        vector<int> tmp = leaf2Examples[leaf2index[branchNow]];
        tmp.push_back(cnt);
        leaf2Examples[leaf2index[branchNow]] = tmp;
        cnt += 1;
    }
    
     int classID = (dataTable[1]).size() - 1;
     for(map<int, int>::iterator it = leaveClass.begin(); it != leaveClass.end(); it++)
     {
     vector<int> expTmp;
     expTmp = leaf2Examples[leaf2index[it->first]];
     int fpSum = 0, fSum = 0, mpSum = 0, mSum = 0;
     for(vector<int>::iterator eit = expTmp.begin(); eit != expTmp.end(); ++eit)
     {
     // senID represents the sensitive feature ID, e.g. 0
     if(dataTable[*eit][senID] == 0){
         fSum += 1;
     if(dataTable[*eit][classID] == 1)
         fpSum +=1;
     }else{
         mSum += 1;
     if(dataTable[*eit][classID] == 1)
         mpSum += 1;
     }
     }
     leaf2Percentage.insert(pair<int, sensitiveInfo>(it->first, make_tuple(fpSum, fSum, mpSum, mSum)));
     }
    
}

double calFair(map<int, sensitiveInfo> &leaf2Percentage, map<int, double> &fairVec)
{
    double sumFair = 0;
    for(map<int, sensitiveInfo>::iterator it = leaf2Percentage.begin(); it != leaf2Percentage.end(); it++)
    {
        double tmp;
        tmp = 1.0 * (get<0>(it->second)) * (get<3>(it->second)) - 0.8 * (get<2>(it->second)) * (get<1>(it->second));
        fairVec.insert(pair<int, double>(it->first, tmp));
        cout << "leave node [" << it->first << "], fair value is " << tmp << endl;
        sumFair += tmp;
    }
    return sumFair;
}

int main(int argc, const char * argv[]) {
    // argv[1] : the file path of the encoding result
    // argv[2] : the dataset represented in the CSV file
    // argv[3] : 0: cal error; 1: cal fairness value;
    // argv[4] : senID
    map<int, int> branch2Feature;
    readFile(argv[1], branch2Feature);
    vector<vector<int>> dataTable;
    genDataTable(argv[2], dataTable);
    vector<int> leaves;
    
    gatherLeaves(branch2Feature, leaves);
    //printDataTable(dataTable);
    map<int, int> leaveClass;
    runClass(dataTable, branch2Feature, leaves, leaveClass);
    printClass(leaveClass);
    
    
    if(!strcmp(argv[3], "0")){
        // Cal Error
        vector<int> Loss;
        int error = calError(branch2Feature, leaves, leaveClass, dataTable, Loss);
    }else{
        // Cal fairness
        // Map: leaf ID => set of information (e.g. the percentage of sensitive features)
        int senID = atoi(argv[4]);
        map<int, sensitiveInfo> leaf2Percentage;
        calLeaf2Percentage(dataTable, branch2Feature, leaveClass, leaf2Percentage, senID);
        map<int, double> fairVec;
        double fair = calFair(leaf2Percentage, fairVec);
    }
    
    return 0;
}
