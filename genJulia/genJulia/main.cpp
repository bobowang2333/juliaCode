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

// path = "/Users/bobobo/Documents/fairSyn/juliaCode/"
void writeJulia(string ID, string path, string ExpNum)
{
    ofstream juliaFile;
    juliaFile.open(path+"level.jl");
    juliaFile << "using JuMP\nusing Gurobi\nusing DataFrames, CSV\n";
    juliaFile << "\n";
    juliaFile << "m = Model(solver = GurobiSolver(NonConvex=2))\n";
    juliaFile << "\n";
    juliaFile << "# set macro, the size of outputClass, input features, .. etc.\n";
    juliaFile << "NE = " + ExpNum + "; # number of examples\n";
    juliaFile << "NF = 5; # number of input features\n";
    juliaFile << "NO = 2; # number of output features\n";
    juliaFile << "\n";
    juliaFile << "# formalize the level\n";
    juliaFile << "nlevel = 2;\n";
    juliaFile << "NL = 2^nlevel; # number of leaves\n";
    juliaFile << "NB = (2^nlevel)-1; # number of branches\n";
    juliaFile << "\n";
    juliaFile << "# assuming the number of classes is NO\n";
    juliaFile << "@variable(m, Lt[1:NL], Int)\n";
    juliaFile << "\n";
    juliaFile << "# bobo : In the one level setting, Am cannot be [0,0,0,..0] vector\n";
    juliaFile << "@variable(m, 0 <= dt[1:NB] <= 1, Int)\n";
    juliaFile << "@variable(m, Nt[1:NL] >= 0, Int)\n";
    juliaFile << "@variable(m, Nkt[1:NL, 1:NO], Int)\n";
    juliaFile << "# bobo : the range of Ckt should also be in [0,1]\n";
    juliaFile << "@variable(m, 0 <= Ckt[1:NL, 1:NO] <= 1, Int)\n";
    juliaFile << "\n";
    juliaFile << "@variable(m, n)\n";
    juliaFile << "# n : number of examples (data points)\n";
    juliaFile << "@constraint(m, n == NE)\n";
    juliaFile << "\n";
    juliaFile << "# 3 constraints for the loss variable Lt\n";
    juliaFile << "@constraint(m, consLoss1[i=1:NL, j=1:NO], Lt[i] >= Nt[i] - Nkt[i,j] - n*(1-Ckt[i,j]))\n";
    juliaFile << "@constraint(m, consLoss2[i=1:NL, j=1:NO], Lt[i] <= Nt[i] - Nkt[i,j] + n*Ckt[i,j])\n";
    juliaFile << "@constraint(m, consLoss3[i=1:NL], Lt[i] >= 0)\n";
    juliaFile << "\n";
    juliaFile << "@variable(m, sumLt, Int)\n";
    juliaFile << "@constraint(m, 0 <= sumLt)\n";
    juliaFile << "@constraint(m, sumLt == sum(Lt[i] for i = 1:NL))\n";
    juliaFile << "\n";
    juliaFile << "@variable(m, Yik[1:NE, 1:NO])\n";
    juliaFile << "@variable(m, 0 <= Zit[1:NE, 1:(NL+NB)] <= 1, Int)\n";
    juliaFile << "\n";
    juliaFile << "# constraint for the root node which has all the exmaples\n";
    juliaFile << "@constraint(m, zitDef[i=1:NE], Zit[i, 1] == 1)\n";
    juliaFile << "\n";
    juliaFile << "@constraintref def1[1:NL, 1:NO]\n";
    juliaFile << "for t = 1:NL\n";
    juliaFile << "\tfor k = 1:NO\n";
    juliaFile << "\t\tdef1[t,k] = @constraint(m, Nkt[t,k] == sum((0.5*Zit[i,t+NB])*(1+Yik[i,k]) for i = 1:NE))\n";
    juliaFile << "\tend\n";
    juliaFile << "end\n";
    juliaFile << "\n";
    juliaFile << "@constraint(m, defNt[t=1:NL], Nt[t] == sum(Zit[i,t+NB] for i = 1:NE))\n";
    juliaFile << "\n";
    juliaFile << "@variable(m, 0 <= lt[1:NL] <= 1, Int)\n";
    juliaFile << "@constraint(m, deflt[t=1:NL], lt[t] == sum(Ckt[t,k] for k = 1:NO))\n";
    juliaFile << "\n";
    juliaFile << "@variable(m, 0 <= Am[1:NF, 1:NB] <= 1, Int)\n";
    juliaFile << "\n";
    juliaFile << "# bobo  : we assume f1 represents the sensitive feature, which cannot be used to split examples\n";
    juliaFile << "@constraintref sensitiveFeature[1:NB]\n";
    juliaFile << "for i = 1:NB\n";
    juliaFile << "\tsensitiveFeature[i] = @constraint(m, Am[1, i] == 0)\n";
    juliaFile << "end\n";
    juliaFile << "\n";
    juliaFile << "# Bm needs to be updated later\n";
    juliaFile << "@variable(m, 0 <= Bm[1:NB] <= 1, Int)\n";
    juliaFile << "\n";
    juliaFile << "@variable(m, Xi[1:NE, 1:NF])\n";
    juliaFile << "@variable(m, tmpRes[1:NE, 1:NB])\n";
    juliaFile << "@constraint(m, Xi * Am .== tmpRes)\n";
    juliaFile << "\n";
    juliaFile << "# constraint for the branch ( left & right side)\n";
    juliaFile << "@variable(m, epilsonMax)\n";
    juliaFile << "@variable(m, epilson[1:NF])\n";
    juliaFile << "@variable(m, XiPlus[1:NE, 1:NF])\n";
    juliaFile << "@variable(m, tmpRes2[1:NE, 1:NB])\n";
    juliaFile << "\n";
    juliaFile << "# epilson and epilson should be computed offline since the result only depdends on the input dataset which is already given.\n";
    juliaFile << "\n";
    juliaFile << "@constraint(m, epilsonDef[i=1:NF], epilson[i] == 1)\n";
    juliaFile << "@constraint(m, epilsonMax == 1)\n";
    juliaFile << "\n";
    juliaFile << "@constraint(m, XiPlusDef[i=1:NE, j=1:NF], XiPlus[i,j] == Xi[i,j] + epilson[j])\n";
    juliaFile << "@constraint(m, XiPlus * Am .== tmpRes2)\n";
    juliaFile << "\n";
    juliaFile << "@constraintref branch[1:(NL+NB), 1:nlevel, 1:NE]\n";
    juliaFile << "for j = 1:(NL+NB)\n";
    juliaFile << "\ttmp::Int32 = j\n";
    juliaFile << "\tcnt::Int32 = 1\n";
    juliaFile << "\twhile tmp > 1\n";
    juliaFile << "\t\ttmpR::Int32 = tmp % 2\n";
    juliaFile << "\t\ttmpD::Int32 = 0\n";
    juliaFile << "\t\tif tmpR != 0\n";
    juliaFile << "\t\t\ttmpD = floor(tmp/2)\n";
    juliaFile << "\t\telse\n";
    juliaFile << "\t\t\ttmpD = tmp/2\n";
    juliaFile << "\t\tend\n";
    juliaFile << "\t\tfor i = 1:NE\n";
    juliaFile << "\t\t\tif tmpR != 0\n";
    juliaFile << "\t\t\t\tbranch[j, cnt, i] = @constraint(m,  tmpRes[i,tmpD] >= Bm[tmpD] - (1 - Zit[i,j]))\n";
    juliaFile << "\t\t\telse\n";
    juliaFile << "\t\t\t\tbranch[j, cnt, i] = @constraint(m, tmpRes2[i,tmpD] <= Bm[tmpD] + (1 + epilsonMax)*(1 - Zit[i,j]))\n";
    juliaFile << "\t\t\tend\n";
    juliaFile << "\t\tend\n";
    juliaFile << "\t\ttmp = tmpD\n";
    juliaFile << "\t\tcnt = cnt + 1\n";
    juliaFile << "\tend\n";
    juliaFile << "end\n";
    juliaFile << "\n";
    juliaFile << "# horizontal dependency\n";
    juliaFile << "@constraintref horizontalSum[1:nlevel, 1:NE]\n";
    juliaFile << "# constraint for the root node which owns all the examples\n";
    juliaFile << "@constraint(m, zitDef[i=1:NE], Zit[i, 1] == 1)\n";
    juliaFile << "\n";
    juliaFile << "for level = 1:nlevel\n";
    juliaFile << "\tfor j = 1:NE\n";
    juliaFile << "\t\thorizontalSum[level, j] = @constraint(m, 1 == sum(Zit[j,t] for t = 2^level:(2^(level+1)-1)))\n";
    juliaFile << "\tend\n";
    juliaFile << "end\n";
    juliaFile << "\n";
    juliaFile << "# Sum{Zit[children]} == Zit[root]  vertical dependency\n";
    juliaFile << "@constraintref sumChildren[1:((2^nlevel)-1)]\n";
    juliaFile << "for i = 1:((2^nlevel)-1)\n";
    juliaFile << "\tfor j = 1:NE\n";
    juliaFile << "\t\tsumChildren[i] = @constraint(m, Zit[j,i] == Zit[j, 2*i] + Zit[j, 2*i+1])\n";
    juliaFile << "\tend\n";
    juliaFile << "end\n";
    juliaFile << "\n";
    juliaFile << "@constraint(m, ZitLt[i=1:NE, t=1:NL], Zit[i,t+NB] <= lt[t])\n";
    juliaFile << "\n";
    juliaFile << "@variable(m, Nmin)\n";
    juliaFile << "@constraint(m, ZitLt2[t=1:NL], sum(Zit[i,t+NB] for i = 1:NE) >= Nmin * lt[t])\n";
    juliaFile << "\n";
    juliaFile << "# Set N_min as 1\n";
    juliaFile << "@constraint(m, Nmin == 1)\n";
    juliaFile << "# t here represents the root/branch node (t = 0)\n";
    juliaFile << "@constraint(m, ajtDt[j=1:NB], dt[j] == sum(Am[i,j] for i = 1:NF))\n";
    juliaFile << "# t represets branch here\n";
    juliaFile << "#@constraint(m, Bm[1:NB] .>= 0)\n";
    juliaFile << "@constraint(m, dtBmDef[i=1:NB], dt[i] >= Bm[i])\n";
    juliaFile << "\n";
    juliaFile << "# now we only consider two levels, so we add constraint dt <= d_{p(t)}\n";
    juliaFile << "numNeed = (2^(nlevel-1))-1\n";
    juliaFile << "@constraintref levelDep[1:numNeed, 1:2]\n";
    juliaFile << "for i = 1:numNeed\n";
    juliaFile << "\tlevelDep[i,1] = @constraint(m, dt[i] >= dt[2*i])\n";
    juliaFile << "\tlevelDep[i,2] = @constraint(m, dt[i] >= dt[2*i+1])\n";
    juliaFile << "end\n";
    juliaFile << "\n";
    juliaFile << "@variable(m, sumDt)\n";
    juliaFile << "@constraint(m, sumDt == sum(dt[i] for i = 1:NB))\n";
    juliaFile << "\n";
    juliaFile << "# If Am and Bm being both 0, then Zit[i,t] == Zit[i, LeftChild(t)]\n";
    juliaFile << "# first branch (7->5), if branch is empty, then all node goes to the right side\n";
    juliaFile << "@constraintref zeroAmBm[1:(2^nlevel-1), 1:NE]\n";
    juliaFile << "for i = 1:(2^nlevel-1)\n";
    juliaFile << "\tfor j = 1:NE\n";
    juliaFile << "\t\tzeroAmBm[i,j] = @constraint(m, Zit[j, i] - Zit[j, 2*i+1] <= dt[i])\n";
    juliaFile << "\tend\n";
    juliaFile << "end\n";
    juliaFile << "\n";
    juliaFile << "#read the input data from CSV file\n";
    juliaFile << "dataID = \"" + ID + "\"\n";
    juliaFile << "dataPath = \"" + path + "\"\n";
    // bobo : assume that the CSV file starts with prefix "test"
    juliaFile << "Dataset = CSV.read(joinpath(Pkg.dir(\"DataFrames\"), dataPath*\"test$dataID.csv\"))\n";
    juliaFile << "# Data matrix without Dataset\n";
    juliaFile << "mt = convert(Matrix, Dataset[:,1:NF])\n";
    juliaFile << "# Data matrix with output\n";
    juliaFile << "output = convert(Vector, Dataset[:, (NF+1)])\n";
    juliaFile << "\n";
    juliaFile << "@constraint(m, readData[i=1:NE, j=1:NF], Xi[i,j] == mt[i,j])\n";
    juliaFile << "\n";
    juliaFile << "# definition of Yik\n";
    juliaFile << "@constraint(m, YikDef0[i=1:NE, j=1:NO ; output[i] == (j - 1)], Yik[i,j] == 1)\n";
    juliaFile << "@constraint(m, YikDef1[i=1:NE, j=1:NO ; output[i] != (j - 1)], Yik[i,j] == -1)\n";
    juliaFile << "# add fairness constraints\n";
    juliaFile << "# FPt represents the number of data points, which is female and the output class is positive.\n";
    juliaFile << "# Ft represents the number of data points, which is female. Similarly for MPt and Mt\n";
    juliaFile << "@variable(m, FPt[1:NE, 1:NL] >= 0, Int)\n";
    juliaFile << "@variable(m, Ft[1:NE, 1:NL] >= 0, Int)\n";
    juliaFile << "@variable(m, MPt[1:NE, 1:NL] >= 0, Int)\n";
    juliaFile << "@variable(m, Mt[1:NE, 1:NL] >= 0, Int)\n";
    juliaFile << "\n";
    juliaFile << "@constraintref FPt_cons[1:NE, 1:NL]\n";
    juliaFile << "for i = 1:NE\n";
    juliaFile << "\tfor j = 1:NL\n";
    juliaFile << "\t\tif(output[i] == 1 && mt[i,1] == 0)\n";
    juliaFile << "\t\t\tFPt_cons[i,j] = @constraint(m, FPt[i,j] == Zit[i, j+NB])\n";
    juliaFile << "\t\telse\n";
    juliaFile << "\t\t\tFPt_cons[i,j] = @constraint(m, FPt[i,j] == 0)\n";
    juliaFile << "\t\tend\n";
    juliaFile << "\tend\n";
    juliaFile << "end\n";
    juliaFile << "\n";
    juliaFile << "@constraintref Ft_cons[1:NE, 1:NL]\n";
    juliaFile << "for i = 1:NE\n";
    juliaFile << "\tfor j = 1:NL\n";
    juliaFile << "\t\tif(mt[i,1] == 0)\n";
    juliaFile << "\t\t\tFt_cons[i,j] = @constraint(m, Ft[i,j] == Zit[i, j+NB])\n";
    juliaFile << "\t\telse\n";
    juliaFile << "\t\t\tFt_cons[i,j] = @constraint(m, Ft[i,j] == 0)\n";
    juliaFile << "\t\tend\n";
    juliaFile << "\tend\n";
    juliaFile << "end\n";
    juliaFile << "\n";
    juliaFile << "@constraintref MPt_cons[1:NE, 1:NL]\n";
    juliaFile << "for i = 1:NE\n";
    juliaFile << "\tfor j = 1:NL\n";
    juliaFile << "\t\tif(output[i] == 1 && mt[i,1] == 1)\n";
    juliaFile << "\t\t\tMPt_cons[i,j] = @constraint(m, MPt[i,j] == Zit[i, j+NB])\n";
    juliaFile << "\t\telse\n";
    juliaFile << "\t\t\tMPt_cons[i,j] = @constraint(m, MPt[i,j] == 0)\n";
    juliaFile << "\t\tend\n";
    juliaFile << "\tend\n";
    juliaFile << "end\n";
    juliaFile << "\n";
    juliaFile << "@constraintref Mt_cons[1:NE, 1:NL]\n";
    juliaFile << "for i = 1:NE\n";
    juliaFile << "\tfor j = 1:NL\n";
    juliaFile << "\t\tif(mt[i,1] == 1)\n";
    juliaFile << "\t\t\tMt_cons[i,j] = @constraint(m, Mt[i,j] == Zit[i, j+NB])\n";
    juliaFile << "\t\telse\n";
    juliaFile << "\t\t\tMt_cons[i,j] = @constraint(m, Mt[i,j] == 0)\n";
    juliaFile << "\t\tend\n";
    juliaFile << "\tend\n";
    juliaFile << "end\n";
    juliaFile << "\n";
    juliaFile << "@variable(m, SumFPt[1:NL] >= 0, Int)\n";
    juliaFile << "@variable(m, SumFt[1:NL] >= 0, Int)\n";
    juliaFile << "@variable(m, SumMPt[1:NL] >= 0, Int)\n";
    juliaFile << "@variable(m, SumMt[1:NL] >= 0, Int)\n";
    juliaFile << "\n";
    juliaFile << "@constraint(m, defSumFPt[t=1:NL], SumFPt[t] == sum(FPt[i, t] for i = 1:NE))\n";
    juliaFile << "@constraint(m, defSumFt[t=1:NL], SumFt[t] == sum(Ft[i, t] for i = 1:NE))\n";
    juliaFile << "@constraint(m, defSumMPt[t=1:NL], SumMPt[t] == sum(MPt[i, t] for i = 1:NE))\n";
    juliaFile << "@constraint(m, defSumMt[t=1:NL], SumMt[t] == sum(Mt[i, t] for i = 1:NE))\n";
    juliaFile << "\n";
    juliaFile << "@variable(m, fairVar[1:NL])\n";
    juliaFile << "# linear fairness constraints\n";
    juliaFile << "@constraint(m, fairDef[t=1:NL], fairVar[t] == SumFPt[t] * SumMt[t] - 0.8 * SumMPt[t] * SumFt[t])\n";
    juliaFile << "\n";
    juliaFile << "@variable(m, fair)\n";
    juliaFile << "@constraint(m, fair == sum(fairVar[t] for t = 1:NL))\n";
    juliaFile << "\n";
    //juliaFile << "@objective(m, Min, sumLt + 0.2 * sumDt - fair)\n";
    juliaFile << "@objective(m, Min, sumLt + 0.2 * sumDt)\n";
    juliaFile << "solve(m)\n";
    juliaFile << "\n";
    juliaFile << "io = open(dataPath*\"res.txt\", \"w\")\n";
    //juliaFile << "io = open(\"/Users/bobobo/Documents/fairSyn/juliaCode/testData/res.txt\", \"w\")\n";
    juliaFile << "choose = 0\n";
    juliaFile << "for i = 1:NF\n";
    juliaFile << "\tif getvalue(Am[i,1]) == 1\n";
    juliaFile << "\t\tchoose = i\n";
    juliaFile << "\tend\n";
    juliaFile << "end\n";
    juliaFile << "println(io, dataID)\n";
    juliaFile << "println(io, choose)\n";
    juliaFile << "close(io)\n";
    juliaFile << "\n";
    juliaFile << "println(\"Final Solution: [ SumFPt : $(getvalue(SumFPt))]\")\n";
    juliaFile << "println(\"Final Solution: [ fairVar : $(getvalue(fairVar))]\")\n";
    juliaFile << "println(\"Final Solution: [ fair : $(getvalue(fair))]\")\n";
    juliaFile << "println(\"Final Solution: [ sumLt : $(getvalue(sumLt))]\")\n";
    juliaFile << "println(\"Final Solution: [ Yik : $(getvalue(Yik))]\")\n";
    juliaFile << "println(\"Final Solution: [ Nt : $(getvalue(Nt))]\")\n";
    juliaFile << "println(\"Final Solution: [ Lt : $(getvalue(Lt))]\")\n";
    juliaFile << "println(\"Final Solution: [ Zit : $(getvalue(Zit))]\")\n";
    juliaFile << "println(\"Final Solution: [ Am : $(getvalue(Am))]\")\n";
    juliaFile << "println(\"Final Solution: [ Bm : $(getvalue(Bm))]\")\n";
    juliaFile.close();
}

int main(int argc, const char * argv[]) {
    cout << "Have " << argc << " arguments:" << endl;
    //int dataID = atoi(argv[1]);
    //string dataPath = "/Users/bobobo/Documents/fairSyn/juliaCode/testData/";
    string dataPath = argv[2];
    // argv[1]: the ID of CSV file, "1", "2", or "3" => "test0.csv"
    // argv[2]: the file path where the generated julia file will be stored
    // argv[3]: the number of examples in the csv file
    writeJulia(argv[1], dataPath, argv[3]);
    //dataPath = dataPath + argv[1];
    //dataPath += ".csv";
    cout << dataPath << endl;
    return 0;
}

