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
    std::string str; 
    int cnt = 0;
    while (std::getline(file, str))
    {
        // Process str
    	cnt++;
    	if(cnt == 2)
    		cout << str << endl;
    }
    return 0;
}