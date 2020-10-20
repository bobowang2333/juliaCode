# Split the CSV file based on the feature and its splitting value 
import csv
import sys

def splitValue(filePath, f_ID, f_v):
	f_left = []
	f_right = []
	first = True
	with open(filePath, 'r') as f:
		reader = csv.reader(f, delimiter=',')
		for row in reader:
			if first:
				f_left.append(row)
				f_right.append(row)
				first = False 
			elif float(row[f_ID-1]) < f_v:
				f_left.append(row)
			elif float(row[f_ID-1]) >= f_v:
				f_right.append(row)
			else:
				continue
	return f_left, f_right

def saveCSV(f_left, f_right, f_ID, runPath):
	left_path = runPath + "/0.csv"
	right_path = runPath + "/1.csv"
	with open(left_path, "w") as csv_file_left:
		writer = csv.writer(csv_file_left)
		for line in f_left:
			#print(line[f_ID-1])
			writer.writerow(line)
	with open(right_path, "w") as csv_file_right:
		writer = csv.writer(csv_file_right)
		for line in f_right:
			writer.writerow(line)

if __name__ == "__main__":
	if len(sys.argv) < 5:
		print("Please specify the input CSV file, the splitting feature, the feature value and the runnning path\n")
	left, right = splitValue(sys.argv[1], int(sys.argv[2]), float(sys.argv[3]))
	# write the left and right side as files "0.csv" and "1.csv"
	saveCSV(left, right, int(sys.argv[2]), sys.argv[4])

