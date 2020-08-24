#!/bin/bash

runPath="/Applications/Julia-0.6.app/Contents/Resources/julia/bin"

#${runPath}/julia /Users/bobobo/Documents/fairSyn/juliaCode/testData/level.jl

currentPath="/Users/bobobo/Documents/fairSyn/juliaCode/testData"
output=$(${currentPath}/readRes ${currentPath}/res.txt)
echo $output

