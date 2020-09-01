//
//  DTL.h
//  DTL
//
//  Created by bobobo on 31/08/2020.
//  Copyright © 2020 bobobo. All rights reserved.
//

#ifndef DTL_h
#define DTL_h

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

struct DTLNode{
    int feature;
    int value; // value for the corresponding feature
    bool isleaf;
    int outputType; // iff the isleaf is satisfied
    vector<DTLNode*> Children;
};

// generate data table from CSV file
void genDataTable(string dataPath, vector<vector<int>> &dataTable);
void initFeature(map<int, int> &availFeature);
double calFeatureProp(vector<vector<int>> &nowTable, int feature, int value);
double calFeatureClass(vector<vector<int>> &nowTable, int feature, int value, int output);
int calEntropy(vector<vector<int>> &nowTable, map<int, int> &availFeature);
double calEntropyExample(vector<vector<int>> &nowTable, int outNum);
int calEntropy(vector<vector<int>> &nowTable, map<int, int> &availFeature, int outNum);
int checkMajority(vector<vector<int>> &nowTable, int outNum);
bool checkSame(vector<vector<int>> &nowTable);
void getNodeExample(DTLNode* node, vector<vector<int>> &nowTable, map<DTLNode*, vector<int>> &RuleNodeExample);
void selectBranchExamples(vector<vector<int>> &nowTable, vector<vector<int>> &splitTable, int feature, int fValue);
DTLNode* DTL_learn(vector<vector<int>> &dataTable, int feature, int fValue, map<int, int> &availFeature, map<DTLNode*, vector<int>> &RuleNodeExample);
void printLevel(int level);
void printDTLTree(DTLNode *treeNode, int level, map<DTLNode*, vector<int>> &RuleNodeExample);
#endif /* DTL_h */
