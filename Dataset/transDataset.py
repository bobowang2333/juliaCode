# input1 : the original dataset
# input2 : the configuraton file specifying which features needed to be discretized
# output : the new booleanized features by one-hot encoding and the discretizaton

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys
from sklearn.tree import DecisionTreeClassifier
from collections import Counter

def readNumData(filePath):
	#data = np.loadtxt(yourFileName,skiprows=n)
	data = np.loadtxt(filePath)
	#print(data)
	return data

def readData(filePath):
	with open(filePath, 'r') as f:
		data = [[num for num in line[:-1].split(' ')] for line in f ]
	return data

def readConfig(filePath):
	dataDict = {}
	with open(filePath, 'r') as f:
		for line in f:
			lineS = line.split()
			if not lineS:  # lineS List is empty
				continue
			dataDict[int(lineS[0])] = [thres for thres in lineS[1:]]
	return dataDict

def boolVal(F):
	newF = np.zeros((len(F), 1))
	neg = F[0]
	for i in range(len(F)):
		if F[i] == neg:
			newF[i,0] = 0
		else:
			newF[i,0] = 1
	return newF

def oneHot(F, counter):
	uniqueSet = list(counter.keys())
	#print(uniqueSet)
	newF = np.zeros((len(F), len(counter.values())))
	for i in range(len(F)):
		for j in range(len(counter.values())):
			if(j == uniqueSet.index(F[i])):
				newF[i,j] = 1
			else:
				newF[i,j] = 0
	return newF

# thres = {0,1}, len = 2, create 3 more columns
def calIndex(v, thres):
	for i in range(len(thres)):
		if v <= thres[0]:
			return 0
		elif v >= thres[len(thres)-1]:
			return len(thres)
		elif thres[i] <= v and v <= thres[i+1]:
			return i
		else:
			continue

def continueous(F, thres):
	newF = np.zeros((len(F), len(thres)+1))
	for i in range(len(F)):
		for j in range(len(thres)+1):
			if(j == calIndex(F[i], thres)):
				newF[i,j] = 1
			else:
				newF[i,j] = 0
	return newF

def genNpMatrix(newData):
	m1 = np.array(newData[0])
	res = m1
	cnt = 0
	#print(res.shape)
	#print(res[0])
	for col in newData:
		if cnt == 0:
			cnt += 1
			continue
		col = np.array(col)
		#print(col.shape)
		res = np.append(res, col, axis=1)
		cnt += 1
	#print(res.shape)
	return res

# generate the dataset with quantiative values
def toNum(F, counter):
	uniqueSet = list(counter.keys())
	newF = np.zeros((len(F), 1))
	for i in range(len(F)):
			newF[i,0] = uniqueSet.index(F[i])
	newF = [(i-newF.min())/(newF.max() - newF.min()) for i in newF]
	return newF


def quantiative(dataArray):
	dataT = [*zip(*dataArray)] 
	newData = list()
	for attri in dataT:
		valueNum = len(Counter(attri).values())
		if valueNum > 2 and valueNum <= 12:
			attriN = toNum(attri, Counter(attri))
			newData.append(attriN)
		elif valueNum == 2:
			attriN = boolVal(attri)
			newData.append(attriN)
		else:
			#numerical value, convert the original list to NP matrix
			attriN = np.zeros((len(attri), 1))
			for i in range(len(attri)):
				attriN[i,0] = attri[i]
			attriN = [(i-attriN.min())/(attriN.max()-attriN.min()) for i in attriN]
			newData.append(attriN)
	newData = genNpMatrix(newData)	
	return newData		


# generate the dataset with boolean values 
def booleanize(dataArray, cDict, x_s):
	dataT = [*zip(*dataArray)] 
	newData = list()
	# iterate all the available features
	cnt = 0
	x_s = 12
	senFile = open("newSenID.txt", "w")
	for attri in dataT:
		#print(len(Counter(attri).values()))
		if len(Counter(attri).values()) > 2 and len(Counter(attri).values()) <= 12:
			# as categorical value and use one-hot encoding
			attriN = oneHot(attri, Counter(attri))
			newData.append(attriN)
		elif len(Counter(attri).values()) == 2:
			# it only has a boolean variable with two possible values
			attriN = boolVal(attri)
			newData.append(attriN)
		else:
			# for the continous value, read threshold from configure file
			#print("cnt : " + str(cnt))
			thres = cDict[cnt]
			attriN = continueous(attri, thres)
			newData.append(attriN)
		# print the number of columns that sensitive attribute can occupy
		if cnt == x_s:
			tmp = genNpMatrix(newData)
			senFile.write(str(len(tmp[0]) - len(attriN[0]))+"\n")
			#print("Sensitive index starts : " + str(len(tmp[0]) - len(attriN[0])))
			#print("Sensitive index length : " + str(len(attriN[0])))
			print(str(len(tmp[0]) - len(attriN[0])))
		cnt += 1
	newData = genNpMatrix(newData)
	senFile.write(str(len(newData[0])))
	senFile.close()
	#print("shape of new DataArray : " + str(newData.shape))
	return newData

# save the matrix as (integer) file 
def saveCSV(dataArray, para):
	#Narray = np.asarray(dataArray)
	#print(dataArray.shape)
	if para == 0:
		np.savetxt("boolean.data", dataArray.astype(int), fmt='%i', delimiter=" ")
	else:
		np.savetxt("boolean.data", dataArray.astype(float), fmt='%.6f', delimiter=" ")


if __name__ == "__main__":
	if len(sys.argv) < 5:
		print('Please input the data file (.CSV or .data), the configuration file, sensitive attribute index (starts from 0), para (0: generate boolean file, 1: generate quantiative file)')
		sys.exit()
	data = readData(str(sys.argv[1]))
	dataDict = readConfig(sys.argv[2])
	para = int(sys.argv[4])
	if para == 0:
		newArray = booleanize(data, dataDict, int(sys.argv[3]))
	else:
		newArray = quantiative(data)
	saveCSV(newArray, para)
