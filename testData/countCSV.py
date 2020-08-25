import csv
import sys

filePath = sys.argv[1]
csvfile = open(filePath, 'r').readlines()
print(len(csvfile) - 1)

