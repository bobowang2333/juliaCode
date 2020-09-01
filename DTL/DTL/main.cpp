//
//  main.cpp
//  DTL
//
//  Created by bobobo on 31/08/2020.
//  Copyright © 2020 bobobo. All rights reserved.
//

#include "/Users/bobobo/Documents/fairSyn/juliaCode/DTL/DTL/DTL.h"

int main(int argc, const char * argv[]) {
    // argv[1]: the file path of the CSV file
    cout << "The CSV file path: " << argv[1] << endl;
    vector<vector<int>> dataTable;
    genDataTable(argv[1], dataTable);
    // starts the DTL learning with feature (empty) = 0 and the feature value is 0
    map<int, int> availableFeature;
    initFeature(availableFeature);
    map<DTLNode*, vector<int>> NodeExample;
    DTLNode *treeNode = new DTLNode;
    treeNode = DTL_learn(dataTable, 0, 0, availableFeature, NodeExample);
    printDTLTree(treeNode, 0, NodeExample);
    return 0;
}


// DataTable, 1st column: data ID; 2nd-6 columns: features; 7 column: output
void genDataTable(string dataPath, vector<vector<int>> &dataTable)
{
    string eachLine;
    ifstream dataFile;
    dataFile.open(dataPath);
    int cnt = 0;
    while (getline(dataFile, eachLine)) {
        cout << eachLine << endl;
        cnt += 1;
        if (cnt == 1)
        {
            // avoid the dataTable takes the header as one line
            continue;
        }
        vector<int> tokens;
        tokens.push_back(cnt);
        stringstream ss(eachLine);
        for (int i; ss >> i; ) {
            tokens.push_back(i);
            if(ss.peek() == ',')
                ss.ignore();
        }
        dataTable.push_back(tokens);
    }
}


// Now we assume that all five features are boolean features
void initFeature(map<int, int> &availFeature)
{
    // TODO : we can change the feature ID number and the range of feature value correspondingly
    // feature value starts from 0 -> 1
    availFeature.insert(std::pair<int, int>(1, 1));
    availFeature.insert(std::pair<int, int>(2, 1));
    availFeature.insert(std::pair<int, int>(3, 1));
    availFeature.insert(std::pair<int, int>(4, 1));
    availFeature.insert(std::pair<int, int>(5, 1));
}


// the probability of feature_i = k among all the examples P(feature_i = k)
double calFeatureProp(vector<vector<int>> &nowTable, int feature, int value)
{
    double cnt = 0;
    double sum = 0;
    for(vector<vector<int>>::iterator it = nowTable.begin(); it != nowTable.end(); it++)
    {
        // omit the "-1 case"
        if((*it)[feature] == -1)
            continue;
        sum++;
        if((*it)[feature] == value)
            cnt++;
    }
    if(sum == 0)
        return 0;
    else
        return cnt/sum;
}

// During examples with feature_i = k, calculate the P(output = t)
double calFeatureClass(vector<vector<int>> &nowTable, int feature, int value, int output)
{
    double sum_feature = 0;
    double sum_output = 0;
    for(vector<vector<int>>::iterator it = nowTable.begin(); it != nowTable.end(); it++)
    {
        int lenData = (*it).size();
        if((*it)[feature] == value){
            sum_feature++;
            if((*it)[lenData-1] == output)
                sum_output++;
        }
    }
    if(sum_feature == 0)
        return 0;
    else
        return sum_output/sum_feature;
}


// output value starts from 0
double calEntropyExample(vector<vector<int>> &nowTable, int outNum)
{
    double count = 0;
    int lenData = nowTable[0].size();
    for(int j = 0; j <= outNum; j++)
    {
        double tmp = calFeatureProp(nowTable, (lenData-1), j);
        tmp = tmp*(1 - tmp);
        count += tmp;
    }
    return count;
}

// TODO : here we assume that the number of output class is 2 (outNum)
int calEntropy(vector<vector<int>> &nowTable, map<int, int> &availFeature, int outNum)
{
    vector<double> entropy;
    map<double, int> entropyID;
    for(map<int, int>::iterator it = availFeature.begin(); it != availFeature.end(); it++)
    {
        int featureID = it->first;
        int availNum = it->second;
        double GiniImpurity;
        double count_feature = 0;
        for(int i = 0; i <= availNum; i++)
        {
            // calculate the entropy for 2 output types (we assume it's boolean), we also assume that the output value starts from 0
            double p1 = calFeatureProp(nowTable, featureID, i);
            double count_output = 0;
            double tmp;
            for(int j = 0; j <= outNum; ++j)
            {
                tmp = calFeatureClass(nowTable, featureID, i, j);
                tmp = tmp*(1 - tmp);
                count_output += tmp;
            }
            GiniImpurity = p1 * count_output;
            count_feature = count_feature + GiniImpurity;
        }
        entropy.push_back(count_feature);
        entropyID.insert(pair<double, int>(count_feature, featureID));
    }
    // select the feature minimizing the entropy
    double minFeature = *std::min_element(entropy.begin(), entropy.end());
    double entropyE = calEntropyExample(nowTable, outNum);
    //cout << entropyE << endl;
    cout << minFeature << endl;
    if(minFeature >= entropyE)
        return 0;
    else
        return entropyID[minFeature];
}

int checkMajority(vector<vector<int>> &nowTable, int outNum)
{
    vector<int> typeCount;
    for(int i = 0; i <= outNum; i++)
        typeCount.push_back(0);
    for(vector<vector<int>>::iterator vit = nowTable.begin(); vit != nowTable.end(); vit++)
    {
        int lenData = (*vit).size();
        typeCount[(*vit)[lenData]] =  typeCount[(*vit)[lenData - 1]] + 1;
    }
    return std::distance(typeCount.begin(), max_element(typeCount.begin(), typeCount.end()));
}

bool checkSame(vector<vector<int>> &nowTable)
{
    int lenData = nowTable[0].size();
    int compare = nowTable[0][lenData - 1];
    for(vector<vector<int>>::iterator vit = nowTable.begin(); vit != nowTable.end(); vit++)
    {
        if(compare != (*vit)[lenData-1])
            return false;
    }
    return true;
}

void getNodeExample(DTLNode* node, vector<vector<int>> &nowTable, map<DTLNode*, vector<int>> &RuleNodeExample)
{
    vector<int> tmp;
    for(vector<vector<int>>::iterator it = nowTable.begin(); it != nowTable.end(); it++)
    {
        tmp.push_back((*it)[0]);
    }
    RuleNodeExample.insert(pair<DTLNode*, vector<int>>(node, tmp));
}

void selectBranchExamples(vector<vector<int>> &nowTable, vector<vector<int>> &splitTable, int feature, int fValue)
{
    for(vector<vector<int>>::iterator it = nowTable.begin(); it != nowTable.end(); it++)
    {
        if((*it)[feature] == fValue)
            splitTable.push_back(*it);
    }
}

DTLNode* DTL_learn(vector<vector<int>> &dataTable, int feature, int fValue, map<int, int> &availFeature, map<DTLNode*, vector<int>> &RuleNodeExample)
{
    // TODO : change the value of outNum (0 - outNum) : the number of features
    int outNum = 1;
    DTLNode *rootNode = new DTLNode;
    rootNode->feature = feature;
    rootNode->value = fValue;
    int lenData = dataTable.empty()? 0 : dataTable[0].size();
    if(dataTable.empty())
        return NULL;
    else if(checkSame(dataTable))
    {
        cout << "They all have the same type " << endl;
        rootNode->isleaf = true;
        vector<DTLNode*> emptySet;
        rootNode->Children = emptySet;
        rootNode->outputType = dataTable[0][lenData-1];
        getNodeExample(rootNode, dataTable, RuleNodeExample);
        return rootNode;
    }else if (availFeature.empty())
    {
        cout << "the majority is " << checkMajority(dataTable, outNum) << endl;
        rootNode->isleaf = true;
        vector<DTLNode*> emptySet;
        rootNode->Children = emptySet;
        rootNode->outputType = checkMajority(dataTable, outNum);
        getNodeExample(rootNode, dataTable, RuleNodeExample);
        return rootNode;
    }else{
        // check the feature with maximal information gain
        int select= calEntropy(dataTable, availFeature, outNum);
        if(!select){
            cout << "the majority is " << checkMajority(dataTable, outNum) << endl;
            rootNode->isleaf = true;
            vector<DTLNode*> emptySet;
            rootNode->Children = emptySet;
            rootNode->outputType = checkMajority(dataTable, outNum);
            getNodeExample(rootNode, dataTable, RuleNodeExample);
            return rootNode;
        }else{
            int availNum = availFeature[select];
            map<int, int> tmpFeature = availFeature;
            tmpFeature.erase(tmpFeature.find(select));
            vector<DTLNode*> ChildrenNode;
            for(int i = 0; i <= availNum; ++i)
            {
                vector<vector<int>> splitTable;
                selectBranchExamples(dataTable, splitTable, select, i);
                DTLNode *childNode = DTL_learn(splitTable, select, i, tmpFeature, RuleNodeExample);
                if (childNode != NULL)
                    ChildrenNode.push_back(childNode);
            }
            rootNode->isleaf = false;
            rootNode->Children = ChildrenNode;
            // Since the node is not leaf, so the output type is 0.
            rootNode->outputType = 0;
            return rootNode;
        }
    }
}

void printLevel(int level)
{
    for(int i = 0; i < level; i++)
    {
        cout << "\t";
    }
}

void printDTLTree(DTLNode *treeNode, int level, map<DTLNode*, vector<int>> &RuleNodeExample)
{
    printLevel(level);
    if(treeNode == NULL){
        cout << "There are some issues in the printing" << endl;
        // Existing feature
    }
    else{
            cout << "Branch [feature] " << treeNode->feature << " = " << treeNode->value << endl;
            if(treeNode->isleaf){
                printLevel(level);
                cout << "Node [output] " << treeNode->outputType << "; Examples : {";
                // print data ID
                vector<int> Children = RuleNodeExample[treeNode];
                for(vector<int>::iterator it = Children.begin(); it != Children.end(); it++)
                {
                    cout << *it << " ";
                }
                cout << "}" << endl;
                return;
            }else{
                level++;
                for(vector<DTLNode*>::iterator it = (treeNode->Children).begin(); it != (treeNode->Children).end(); it++)
                {
                    printDTLTree(*it, level, RuleNodeExample);
                }
        }
    }
}













