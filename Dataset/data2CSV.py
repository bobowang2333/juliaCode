# input : data file, each row is of data
# output : CSV file with header "f1,f2,f3..."

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys
from sklearn.tree import DecisionTreeClassifier
import csv
import random

def readData(filePath):
	para = 1
	with open(filePath, 'r') as f:
		if not para: # boolean dataset
			data = [[int(num) for num in line[:-1].split(' ')] for line in f ]
		else: # quantiative dataset
			data = [[float(num) for num in line[:-1].split(' ')] for line in f ]
	print("Feature Num : " + str(len(data[0])))
	return data

def genList(n):
	name = list()
	for i in range(n-1):
		name.append("f"+str(i+1))
	name.append("output")
	print(name)
	return name

def writeCSV(dataAray, outName, randPara):
	headName = genList(len(dataAray[0]))
	data = list(range(0, len(dataAray)))
	random.shuffle(data)
	with open(outName, 'w', newline='') as outfile:
		writer = csv.writer(outfile)
		writer.writerow(headName)
		if randPara == 1:
			print("sampling the dataset")
			for i in range(int(len(dataAray)/50)):
				writer.writerow(dataAray[i])
			testArray = list()
			for i in range(int(len(dataAray)/50), int(len(dataAray)/25)):
				testArray.append(dataAray[i])
			return headName, testArray
		else:
			print("output the original dataset")
			for row in dataAray:
				writer.writerow(row)

def test2CSV(HeadName, testArray, outName):
	with open(outName, 'w', newline='') as outfile:
		writer = csv.writer(outfile)
		writer.writerow(headName)
		for i in testArray:
			writer.writerow(i)

if __name__ == "__main__":
	if len(sys.argv) < 5:
		print("Please input the non-header .data file, the name of output with header CSV file,  the parameter (decide if to sample) and the name of TEST file")
	# argv[3] : "0" -> represent not sample the data; "1"  -> represent to sample the data
	dataAray = readData(sys.argv[1])
	randPara = int(sys.argv[3])
	if randPara == 0:
		writeCSV(dataAray, sys.argv[2], randPara)
	else:
		headName, testArray = writeCSV(dataAray, sys.argv[2], randPara) # generate training set in name "test1.csv"
		test2CSV(headName, testArray, sys.argv[4]) # generate test set in name "TESTd"
