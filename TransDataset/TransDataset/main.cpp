//
//  main.cpp
//  TransDataset
//
//  Created by bobobo on 04/09/2020.
//  Copyright © 2020 bobobo. All rights reserved.
//

#include "/Users/bobobo/Documents/fairSyn/juliaCode/TransDataset/TransDataset/format.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    // argv[1]: the path of the test dataset
    // argv[2] : the path of the new boolean dataset
    vector<vector<int>> dataTable, transpose;
    genDataTable(argv[1], dataTable);
    // the number of various values for each feature
    vector<int> featureValue;
    transposeMatrix(dataTable, transpose);
    vector<vector<float>> newColumnTable, newDataTable;
    genFeatureValue(transpose, featureValue, newColumnTable);
    transposeMatrixFloat(newColumnTable, newDataTable);
    write2CSV(newDataTable, argv[2]);
    return 0;
}

void write2CSV(vector<vector<float>> newDataTable, string filePath)
{
    std::ofstream outfile;
    outfile.open(filePath);
    // output the "f1,f2....out" title
    int len = newDataTable[0].size();
    for(int i = 1; i < len; ++i)
    {
        outfile << "f" << i << ",";
    }
    outfile << "out,\n";
    // output the data
    for(vector<float> row : newDataTable)
    {
        for(float value : row)
        {
            outfile << value << ",";
        }
        outfile << "\n";
    }
    outfile.close();
}


void genFeatureValue(vector<vector<int>> &dataTable, vector<int> &featureValue, vector<vector<float>> &newColumnTable)
{
    int cnt = 0;
    int len = dataTable.size();
    int exampleNum = dataTable[0].size();
    vector<int> featureAverage;
    calDatasetGini(dataTable[len-1]);
    for(int i = 1; i < len-1; ++i)
    {
        cnt++;
        vector<int> nowColumn = dataTable[i];
        vector<int> uniqueColumn;
        std::sort(nowColumn.begin(), nowColumn.end());
        int uniqueCount = std::unique(nowColumn.begin(), nowColumn.end()) - nowColumn.begin();
        
        std::sort(nowColumn.begin(), nowColumn.end());
        featureValue.push_back(uniqueCount);
        // print the feature with the corresponding number of possible values
        
        cout << "Feature [" << cnt << "], range [" << uniqueCount << "]" << endl;
        
        // If the range of possible values is less than 11, then we have 11 splittings. {(3), (1,2,4,5,...11)}
        vector<float> newColumn;
        cout << "Current Feature [" << i << "]" << endl;
        /*
        if(uniqueCount <= 11)
        {
            discreteSplit(nowColumn, dataTable[i], dataTable[len-1], newColumn);
        }
         */
        
        // one-hot encoding for the value range which is less than or equivalent to 4
        if(uniqueCount <= 4)
        {
            vector<vector<float>> newColumnSet;
            oneHotSplit(dataTable[i], dataTable[len-1], newColumnSet);
            for(vector<float> column : newColumnSet)
                newColumnTable.push_back(column);
        }
        /*
        else{
            // then we use (a_i+a_(i+1))/2 as the splitting point
            continueSplit(nowColumn, dataTable[i], dataTable[len-1], newColumn);
            newColumnTable.push_back(newColumn);
        }
         */
        else{
            // then we normalize the original value to 0-1 interval
            normalizeColumn(dataTable[i], newColumn);
            newColumnTable.push_back(newColumn);
        }
        
        /*
        // Stop Calculating the frequency
        // Calculate the frequency
        std::map<int, int, std::greater<int>> value2Freq;
        calFrequency(sortedColumn, value2Freq);
        printFreq(value2Freq, exampleNum);
        
        // print the feature values if the range is less than 10
        if(uniqueCount <= 10)
        {
            set<int> s( nowColumn.begin(), nowColumn.end() );
            nowColumn.assign( s.begin(), s.end() );
            cout << "Feature [" << cnt << "] -> {" ;
            for(int iter : nowColumn)
                cout << iter << " ";
            cout << "}" << endl;
        }
        */
    }
    
    //newColumnTable.push_back(dataTable[len-1]);
    vector<float> tmpVec;
    for(int i : dataTable[len-1])
    {
        tmpVec.push_back(i*1.0);
    }
    newColumnTable.push_back(tmpVec);
    
    // Check the average value
}

// input : original vector is sorted
void genUniqueVector(vector<int> &original, vector<int> &newVector)
{
    int start = original[0];
    newVector.push_back(start);
    for(int i : original)
    {
        if(i != start)
        {
            newVector.push_back(i);
            start = i;
        }
    }
}

void normalizeColumn(vector<int> &nowColumn, vector<float> &newColumnSet)
{
    // the input : "nowColumn" is in the original form and not sorted, and also should not be empty
    int maxValue = nowColumn[0];
    int minValue = nowColumn[0];
    for(int i : nowColumn)
    {
        if(i > maxValue)
            maxValue = i;
        if(i < minValue)
            minValue = i;
    }
    
    // write the normalized value to new vector
    // x_n = x - Min/ (Max - Min)
    for(int i : nowColumn)
    {
        float ni = (i - minValue) * 1.0 / (maxValue - minValue) * 1.0;
        newColumnSet.push_back(ni);
    }
}

void oneHotSplit(vector<int> &nowColumn, vector<int> &output, vector<vector<float>> &newColumnSet)
{
    vector<int> uniqueValue; // the set of unique values {1,2,3...}
    for(int i : nowColumn)
    {
        if(find(uniqueValue.begin(), uniqueValue.end(), i) == uniqueValue.end())
            uniqueValue.push_back(i);
    }
    
    int range = uniqueValue.size();
    for(int i = 0; i < range; ++i)
    {
        vector<float> newColumn;
        for(int j : nowColumn)
        {
            if(j != uniqueValue[i])
                newColumn.push_back(0.0);
            else
                newColumn.push_back(1.0);
        }
        newColumnSet.push_back(newColumn);
    }
}

void discreteSplit(vector<int> &nowColumn, vector<int> &unsortedColumn, vector<int> &output, vector<int> &newColumn)
{
    // get a list of unique elements
    vector<int> uniqueColumn;
    //set<int> s( nowColumn.begin(), nowColumn.end() );
    //uniqueColumn.assign( s.begin(), s.end() );
    genUniqueVector(nowColumn, uniqueColumn);
    double minGini = 1.0  ;
    int minGiniFeature = 0;
    for(int i : uniqueColumn)
    {
        // calculate the corrspoding Gini impurity for the corresponding feature
        vector<int> pColumn, nColumn;
        int ID = 0;
        for(int element : nowColumn)
        {
            int outValue = output[ID];
            // splitting feature
            if(element == i)
                pColumn.push_back(outValue);
            else
                nColumn.push_back(outValue);
            ID++;
        }
        double res = calGini(pColumn, nColumn);
        if(res < minGini)
        {
            minGini = res;
            minGiniFeature = i;
        }
    }
    
    // after choosing the corresponding feature with minimal Gini impurity, rewrite the Column vector
    int cntPostive = 0;
    int cntNegative = 0;
    for(int value : unsortedColumn)
    {
        if(value == minGiniFeature){
            newColumn.push_back(1);
            cntPostive++;
        }
        else{
            newColumn.push_back(0);
            cntNegative++;
        }
    }
    cout << "Select Feature Splitting : " << minGiniFeature << " ; mini Gini Value : " << minGini << endl;
    cout << "after feature splitting, the positive amount : " << cntPostive << " ; The negative amount : " << cntNegative << endl;
}

void continueSplit(vector<int> &nowColumn, vector<int> &unsortedColumn, vector<int> &output, vector<int> &newColumn)
{
    // get a list of unique elements
    vector<int> uniqueColumn;
    //set<int> s( nowColumn.begin(), nowColumn.end() );
    //uniqueColumn.assign( s.begin(), s.end() );
    genUniqueVector(nowColumn, uniqueColumn);
    double minGini = 1.0;
    int minGiniFeature = 0;
    int cnt = 0;
    int ai = uniqueColumn[0];
    for(int i : uniqueColumn)
    {
        if(cnt <= 0){
            cnt++;
            continue;
        }
        int feature = (ai + i)/2;
        ai = i;
        vector<int> pColumn, nColumn;
        int ID = 0;
        for(int element : nowColumn)
        {
            int outValue = output[ID];
            // splitting feature
            if(element <= feature)
                pColumn.push_back(outValue);
            else
                nColumn.push_back(outValue);
            ID++;
        }
        // If the splitting is skew, then we skip it
        if((pColumn.size() <= 10) || (nColumn.size() <= 10))
            continue;
        double res = calGini(pColumn, nColumn);
        if(res < minGini)
        {
            minGini = res;
            minGiniFeature = feature;
        }
    }
    
    if (minGiniFeature == 0)
        cout << "Failure in Selection: no feature can have a non-skew splitting" << endl;
    
    // after choosing the corresponding feature with minimal Gini impurity, rewrite the Column vector
    int cntPostive = 0;
    int cntNegative = 0;
    for(int value : unsortedColumn)
    {
        if(value <= minGiniFeature){
            newColumn.push_back(1);
            cntPostive++;
        }
        else{
            newColumn.push_back(0);
            cntNegative++;
        }
    }
    cout << "Select Feature Splitting : " << minGiniFeature << " ; mini Gini Value : " << minGini << endl;
    cout << "after feature splitting, the positive amount : " << cntPostive << " ; The negative amount : " << cntNegative << endl;
}

void calDatasetGini(vector<int> &output)
{
    int cntP = 0;
    int sum = output.size();
    for(int i : output)
    {
        if(i)
            cntP++;
    }
    double res;
    res =  1.0 - (cntP*1.0/sum)*(cntP*1.0/sum) - (1.0 - cntP*1.0/sum)*(1.0 - cntP*1.0/sum);
    cout << "The Gini impurity of examples is " << res << endl;
}

double calGini(vector<int> &pColumn, vector<int> &nColumn)
{
    int pL = pColumn.size();
    int nL = nColumn.size();
    int sum = pL + nL;
    int cntP = 0;
    for(int value : pColumn)
    {
        if(value)
            cntP++;
    }
    int cntN = 0;
    for(int value : nColumn)
    {
        if(value)
            cntN++;
    }
    double res, resP, resN;
    resP = 1.0 - (cntP*1.0/pL)*(cntP*1.0/pL) - (1.0 - cntP*1.0/pL)*(1.0 - cntP*1.0/pL);
    resN = 1.0 - (cntN*1.0/nL)*(cntN*1.0/nL) - (1.0 - cntN*1.0/nL)*(1.0 - cntN*1.0/nL);
    res = (1.0*pL/sum) * resP * 1.0 + (1.0*nL/sum) * resN * 1.0;
    return res;
}

void printFreq(map<int, int, std::greater<int>> &m, int all)
{
    // only print the first 3 classes
    int cnt = 0;
    for(map<int, int>::iterator it = m.begin(); it != m.end(); ++it)
    {
        double Percentage = ((it->first)*1.0)/all;
        if(cnt > 10)
            break;
        cout << "Feature Value " << it->second << " ; Frequency Percentage : " << Percentage << endl;
        cnt++;
    }
}

void calFrequency(vector<int> &nowColumn, map<int, int, std::greater<int>> &m)
{
    int nowValue = nowColumn[0];
    int cnt = 0;
    for(int elem : nowColumn)
    {
        if(nowValue != elem)
        {
            m.insert(pair<int, int>(cnt, nowValue));
            nowValue = elem;
            cnt = 0;
        }else{
            cnt += 1;
        }
    }
    m.insert(pair<int, int>(cnt, nowValue));
}

void transposeMatrix(vector<vector<int>> &dataTable, vector<vector<int>> &transpose)
{
    int len = dataTable[0].size();
    cout << "The data has " << dataTable.size() << " samples; Each one has " << len << " features. " << endl;
    for(int i = 0; i < len; ++i)
    {
        vector<int> emptyVec;
        transpose.push_back(emptyVec);
    }
    for(vector<vector<int>>::iterator vvit = dataTable.begin(); vvit != dataTable.end(); ++vvit)
    {
        vector<int> current = *vvit;
        for(int i = 0; i < len ; ++i)
        {
            transpose[i].push_back(current[i]);
        }
    }
}

void transposeMatrixFloat(vector<vector<float>> &dataTable, vector<vector<float>> &transpose)
{
    int len = dataTable[0].size();
    cout << "The data has " << dataTable.size() << " samples; Each one has " << len << " features. " << endl;
    for(int i = 0; i < len; ++i)
    {
        vector<float> emptyVec;
        transpose.push_back(emptyVec);
    }
    for(vector<vector<float>>::iterator vvit = dataTable.begin(); vvit != dataTable.end(); ++vvit)
    {
        vector<float> current = *vvit;
        for(int i = 0; i < len ; ++i)
        {
            transpose[i].push_back(current[i]);
        }
    }
}

// DataTable, 1st column: data ID; Last Column: output class ; Data starts from the 3rd row
void genDataTable(string dataPath, vector<vector<int>> &dataTable)
{
    string eachLine;
    ifstream dataFile;
    dataFile.open(dataPath);
    int cnt = 0;
    while (getline(dataFile, eachLine)) {
        //cout << eachLine << endl;
        cnt += 1;
        if (cnt <= 2)
        {
            // avoid the dataTable takes the header as one line
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

