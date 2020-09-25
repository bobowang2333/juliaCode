#!/bin/bash

interactive=
filename="User didnt specify the input dataset"
runPath="/Users/bobobo/Documents/fairSyn/juliaCode/testData"
juliaPath="/Applications/Julia-0.6.app/Contents/Resources/julia/bin"

# need to modify for each Dataset
senID=2

# e.g. DatasetName = ("1", "2", ...) here we assume file name is "test0.csv", "test1.csv", which has been hard coded in genJulia.cpp 
declare -a DatasetName=()
# "1:0", "1" represents the ID for the root branch
declare -a branch2Feature=()

while [ "$1" != "" ]; do
    case $1 in
        -f | --file )   shift 
                        filename=$1
                        ;;
        -h | --help )   usage
                        exit
                        ;;
        * )             usage
                        exit 1
        esac
        shift
done

echo "FileName: ${filename}"
DatasetName+=($filename)

while [ "${#DatasetName[@]}" -ne 0 ]
do
    # step1: run the python/C++ script to generate the julia file given dataset 
    # Function Split(int Feature, string Dataset), output the julia file "level.jl"
    echo "-------Generate the Julia file from CSV--------------"
    branch=${DatasetName[@]:0:1}
    csvName="${runPath}/test${branch}.csv"
    # count the line number of CSV file
    lineNum=$(python ${runPath}/countCSV.py ${csvName})
    ${runPath}/genJulia $branch ${runPath}/ $lineNum $senID

    # step2: run the julia script to get the feature ID "res.txt"
    # /Applications/Julia-0.6.app/Contents/Resources/julia/bin/julia /Users/bobobo/Documents/fairSyn/juliaCode/Encoding.jl
    echo "-------- Invoking Gurobi Solver to solve the MIP encoding ---------------"
    ${juliaPath}/julia ${runPath}/level.jl

    # step3: split the dataset based on the feature set
    # 3.1 build the branch-to-feature mapping
    # 3.2 Function GenData(res.txt, string Dataset), output the prefix of two files'name  and then add "_1.csv", "_2.csv" as name

    outputFeature=$(${runPath}/readRes ${runPath}/res.txt)
    if [ $outputFeature != 0 ]; then
        echo "-------- current branch ${branch} keeps splitting ----------"
        tmp="${branch}:${outputFeature}"
        branch2Feature+=($tmp)
        sourceCSV="${runPath}/test${branch}.csv"
        python ${runPath}/splitGroup.py $sourceCSV $(( outputFeature - 1))
        leftCSV="${runPath}/test$((branch * 2)).csv"
        rightCSV="${runPath}/test$((branch * 2 + 1)).csv"
        mv ${runPath}/0.csv ${leftCSV}
        mv ${runPath}/1.csv ${rightCSV}
    else
        echo "-------- current branch ${branch} has finished splitting and become leave ----------"
    fi
    # step4: add the fileNames to the DatasetName array. Before adding to the name array, check if the fileName is empty
    echo "---------Finish building branch ${branch} -------------------"
    delete=$branch
    new_array=()
    for value in "${DatasetName[@]}"
    do
        [[ $value != $delete ]] && new_array+=($value)
    done
    if [ $outputFeature != 0 ]; then
        echo "-------- Adding subBranches ------------"
        new_array+=($((branch * 2)))
        new_array+=($((branch * 2 + 1)))
    fi
    DatasetName=("${new_array[@]}")
    unset new_array
    # TODO build the mapping between node ID to feature ID, in order to reconstruct the whole tree
    # tmp="branchID::featureID"  # read from the res.txt file
    # branch2Feature+=($tmp)

done

echo "start branch to feature result ---------------------"
# print branch to feature result
for val in "${branch2Feature[@]}" ; do
    KEY=${val%%:*}
    VALUE=${val#*:}
    printf "%s -> %s\n"  "$KEY" "$VALUE" >> ${runPath}/treeRes.txt
done
