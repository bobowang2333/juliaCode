# input : the set of dataset (in CSV format) , the output of which is modified by the class node of decision tree
# output : the value of the DTDI

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys
from sklearn.tree import DecisionTreeClassifier
import random

# for example, filePath = "/Users/jingbow/Documents/CV5.csv"
def readCSV(filePath):
	res = pd.read_csv(filePath)
	resArray = res.values
	# resArray[0:] for dataset, the first row starts with data
	# resArray[1:] for dataset, the second row starts with data
	resArray = resArray[1:]
	print(len(resArray))
	return resArray

def list2NPmatrix(resArray):
	x_len = len(resArray)
	y_len = len(resArray[0])
	output = np.zeros((x_len, y_len))
	for i in range(x_len):
		output[i,:] = resArray[i,:]
	return output

def readNumData(filePath):
	#data = np.loadtxt(yourFileName,skiprows=n)
	resArray = np.loadtxt(filePath)
	return resArray

def eculideanDistance(rowD1, rowD2):
	distance = 0.0
	for i in range(len(rowD1)-1):
		distance += (rowD1[i] - rowD2[i])**2
	return np.sqrt(distance)

def calKNN(resArray, kNeigh):
	dataDict = {}
	cntI = 0
	for dataI in resArray:
		# the data item being calculated is i-th data
		distances = list()
		cntJ = 0;
		for dataJ in resArray:
			# TODO when calculating distance(x_5, x_1), it can be omitted since distance(x_1, x_5) has been calculated already
			dist = eculideanDistance(dataI, dataJ);
			# omit the same data vector
			if cntI == cntJ:
				cntJ += 1
				continue
			distances.append((cntJ, dist))
			cntJ += 1
		distances.sort(key=lambda tup: tup[1])
		neighbours = list()
		for i in range(kNeigh):
			neighbours.append(distances[i][0])
		dataDict[cntI] = neighbours
		#print("Calculating neighbours of Row " + str(cntI))
		cntI += 1
	return dataDict

# x_s : the sensitive feature, v_s : the value corresponding to the sensitive feature
# list_j : the KNN set of data point j
def singelDTDI(resArray, output, x_s, v_s, x_j, list_j):
	d1 = len(list_j)
	d1y = 0
	for i in list_j:
		data_i = resArray[i]
		if data_i[len(data_i)-1] == output:
			d1y += 1
	d2p = 0
	d2y = 0
	for i in list_j:
		data_i = resArray[i]
		if data_i[x_s] == v_s:
			d2p += 1
			if data_i[len(data_i)-1] == output:
				d2y += 1
	if d2p == 0:
		return d1y/d1*1.0 - 0.0
	else:
		return d1y/d1*1.0 - d2y/d2p*1.0

def calDTDI(resArray, x_s):
	o_ind = len(resArray[0])-1
	# get transpose of the original data
	resArrayT = [*zip(*resArray)] 
	# get the unique item in the output vector
	outputU = set(resArrayT[o_ind]) 
	senU = set(resArrayT[x_s])
	dataN = len(resArrayT[0])
	# calculate the KNN for each data point
	knnDict = calKNN(resArray, 5);
	res = 0.0
	for out in outputU:
		for v_s in senU:
			for x_j in range(dataN):
				# calculate the KNN set of x_j
				list_j = knnDict[x_j]
				res += singelDTDI(resArray, out, x_s, v_s, x_j, list_j)
	return res

def splitXY(XYarray):
	x_len = len(XYarray[0])-1
	XYarray = np.array(XYarray)
	x_train = XYarray[:, 0:(x_len-1)]
	y_train = XYarray[:, x_len]
	return x_train, y_train

def mergeXY(xArray, yArray):
	S = xArray.shape
	merge = np.zeros((S[0], S[1]+1))
	merge[:, 0:-1] = xArray
	merge[:, S[1]] = yArray
	print(merge.shape)
	#return np.append(xArray, yArray, 1)
	return merge

def randomFold(resArray, num):
	print(len(resArray))
	data = list(range(0, num))
	random.shuffle(data)
	testArray = list()
	trainArray = list()
	for i in range(int(num/5)):
		testArray.append(resArray[data[i]])
	for j in range(int(num/5), num):
		trainArray.append(resArray[data[j]])
	return trainArray, testArray

def runTest(resArray):
	lenData = len(resArray)
	print(lenData)
	trainArray, testArray = randomFold(resArray, lenData)
	clf = CART(trainArray)
	x_test, y_test = splitXY(testArray)
	y_predict = clf.predict(x_test)
	return mergeXY(x_test, y_predict)

def CART(resArray):
	clf = DecisionTreeClassifier(max_depth = 10,  random_state = 0)
	x_train, y_train = splitXY(resArray)
	clf.fit(x_train, y_train) 
	return clf


if __name__ == "__main__":
	if len(sys.argv) < 4:
		print('Please input the result CSV file with predicted output, the ID of the sensitive feature and parameter (0: run CART library function; 1: use the predicted dataset to calculate the DTDI result)')
		sys.exit()
	if(int(sys.argv[3]) == 0):
		resArray = readCSV(str(sys.argv[1]))
		#resArray = readNumData(str(sys.argv[1]))
		x_s = int(sys.argv[2]) # second column
		testArray = runTest(resArray)
		print(testArray.shape)
		print('DTDI value for decision tree: ', str(calDTDI(testArray, x_s)))
	else:
		# use the predicted data CSV file to calculate the DTDI value
		resArray = list2NPmatrix(readCSV(sys.argv[1]))
		x_s = 12
		print('DTDI value for decision tree: ', str(calDTDI(resArray, x_s)))



