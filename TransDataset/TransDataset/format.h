//
//  format.h
//  TransDataset
//
//  Created by bobobo on 04/09/2020.
//  Copyright © 2020 bobobo. All rights reserved.
//

#ifndef format_h
#define format_h

#include <string>
#include <tuple>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <numeric>
using namespace std;


void genFeatureValue(vector<vector<int>> &dataTable, vector<int> &featureValue, vector<vector<float>> &newColumnTable);
void write2CSV(vector<vector<float>> newDataTable, string filePath);
void genUniqueVector(vector<int> &original, vector<int> &newVector);

void normalizeColumn(vector<int> &nowColumn, vector<float> &newColumnSet);

void oneHotSplit(vector<int> &nowColumn, vector<int> &output, vector<vector<float>> &newColumnSet);

void discreteSplit(vector<int> &nowColumn, vector<int> &unsortedColumn, vector<int> &output, vector<int> &newColumn);
void continueSplit(vector<int> &nowColumn, vector<int> &unsortedColumn, vector<int> &output, vector<int> &newColumn);
void calDatasetGini(vector<int> &output);
double calGini(vector<int> &pColumn, vector<int> &nColumn);
void printFreq(map<int, int, std::greater<int>> &m, int all);
void calFrequency(vector<int> &nowColumn, map<int, int, std::greater<int>> &m);
void transposeMatrix(vector<vector<int>> &dataTable, vector<vector<int>> &transpose);
void transposeMatrixFloat(vector<vector<float>> &dataTable, vector<vector<float>> &transpose);
void genDataTable(string dataPath, vector<vector<int>> &dataTable);

#endif /* format_h */
