# input: treeRes.txt denoting the result 
# output: the matrix with predicted output as result 
import sys
import pandas as pd
import csv

def readCSV(filePath):
	res = pd.read_csv(filePath)
	resArray = res.values
	# resArray[0:] for dataset, the first row starts with data
	# resArray[1:] for dataset, the second row starts with data
	resArray = resArray[1:]
	return resArray

def readResult(filePath, param):
	BranchDict = {}
	ValueDict = {}
	with open(filePath) as f:
		lines = f.readlines()
	content = [i.strip() for i in lines]
	#print(content)
	if param:
		num = int(len(content)/2)
	else:
		num = len(content)
	for i in range(num):
		words = content[i].split("->") 
		# the feature ID in the encoding is 1-index while our list is 0-index
		BranchDict[int(words[0])] = int(words[1])-1
		if param == 0:
			ValueDict[int(words[0])] = 1
	for i in range(num):
		if param:
			words = content[num+i].split("->")
			ValueDict[int(words[0])] = float(words[1])
	return BranchDict, ValueDict

def runTree(item, BranchDict, ValueDict):
	node = 1
	while BranchDict.get(node) != None:
		f = BranchDict[node]
		f_v = ValueDict[node]
		#print(f)
		if item[f] < f_v:
			node = node * 2
		else:
			node = node * 2 + 1
	return node

def getClass(trainData, BranchDict, ValueDict):
	leafDict = {}
	for row in trainData:
		node = runTree(row, BranchDict, ValueDict)
		if leafDict.get(node) != None:
			if row[-1] > 0:
				leafDict[node] += 1
			else:
				leafDict[node] -= 1
		else:
			if row[-1] > 0:
				leafDict[node] = 1
			else:
				leafDict[node] = -1
	for key in leafDict.keys():
		if leafDict[key] >= 0:
			leafDict[key] = 1
		else:
			leafDict[key] = 0
	return leafDict

def getResult(testData, BranchDict, ValueDict, leafDict):
	newData = list()
	for row in testData:
		node = runTree(row, BranchDict, ValueDict)
		out = leafDict[node]
		row[-1] = out
		newData.append(row)
	return newData

def saveCSV(filePath, newData):
	with open(filePath, "w") as f:
		writer = csv.writer(f)
		for row in newData:
			writer.writerow(row)


if __name__ == "__main__":
	if len(sys.argv) < 6:
		print("Please specify the input result file(treeRes), training dataset, test dataset, the output file name and the param value (1: quantiative; 0: boolean)")
	BranchDict, ValueDict = readResult(sys.argv[1], int(sys.argv[5]))
	data = readCSV(sys.argv[2])
	leafDict = getClass(data, BranchDict, ValueDict)
	testData = readCSV(sys.argv[3])
	newData = getResult(testData, BranchDict, ValueDict, leafDict)
	saveCSV(sys.argv[4], newData)

