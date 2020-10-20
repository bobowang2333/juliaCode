//
//  main.cpp
//  genJulia
//
//  Created by bobobo on 21/08/2020.
//  Copyright © 2020 bobobo. All rights reserved.
//
// Input: the dataset in CSV form
// Output: the julia file which needs to be solved by the solver

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>
using namespace std;


int main(int argc, const char * argv[]) {
    //cout << "Have " << argc << " arguments:" << endl;
    //int dataID = atoi(argv[1]);
    std::ifstream file(argv[1]);
    std::string senID, featureNum, str; 
    int cnt = 0;
    while (std::getline(file, str))
    {
        // argv[1] : the name of the input file
        // argv[2]: 0 -> print feature ID ; 1-> print feature value
        // Process str
    	cnt++;
    	if(cnt == 1)
            senID = str;
        if(cnt == 2)
            featureNum = str;
    }
    if(atoi(argv[2]) == 0)
        cout << senID << endl;
    else
        cout << featureNum << endl;
    return 0;
}
