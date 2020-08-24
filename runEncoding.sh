#!/bin/bash

interactive=
filename="User didnt specify the input dataset"
runPath="/Users/bobobo/Documents/fairSyn/juliaCode/testData"
juliaPath="/Applications/Julia-0.6.app/Contents/Resources/julia/bin"

# e.g. DatasetName = ("1", "2", ...)
declare -a DatasetName=()
# "1:0", "1" represents the ID for the root branch
declare -a branch2Feature=("0:0", "1:0")

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
    # step1: run the python/C++ script to split the dataset 
    # Function Split(int Feature, string Dataset), output the julia file "level.jl"

    branch=${Datasetname[@]:0:1}
    #Split(${branch2Feature[$branch]#*:}, $branch)
    ${runPath}/genJulia $branch

    # step2: run the julia script to get the feature ID "res.txt"
    # /Applications/Julia-0.6.app/Contents/Resources/julia/bin/julia /Users/bobobo/Documents/fairSyn/juliaCode/Encoding.jl

    ${juliaPath}/julia ${runPath}/level.jl

    # step3: split the dataset based on the feature set
    # 3.1 build the branch-to-feature mapping
    # 3.2 Function GenData(res.txt, string Dataset), output the prefix of two files'name  and then add "_1.csv", "_2.csv" as name

    outputFeature=$(${runPath}/readRes ${runPath}/res.txt) 
    tmp="${branch}::${outputFeature}"
    branch2Feature+=($tmp)

    # step4: add the fileNames to the DatasetName array. Before adding to the name array, check if the fileName is empty

    # TODO build the mapping between node ID to feature ID, in order to reconstruct the whole tree
    # tmp="branchID::featureID"  # read from the res.txt file
    # branch2Feature+=($tmp)

done

