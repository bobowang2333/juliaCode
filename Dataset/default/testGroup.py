import sys
import csv

def splitGroup(filePath, featureID):
    with open(filePath) as fin:    
        csvin = csv.DictReader(fin)
        # Category -> open file lookup
        outputs = {}
        for row in csvin:
            columName = "f" + featureID 
            cat = row[columName]
            # Open a new file and write the header
            if cat not in outputs:
                fout = open('{}.csv'.format(cat), 'w')
                dw = csv.DictWriter(fout, fieldnames=csvin.fieldnames)
                dw.writeheader()
                outputs[cat] = fout, dw
            # Always write the row
            outputs[cat][1].writerow(row)
        # Close all the files
        for fout, _ in outputs.values():
            fout.close()

if __name__ == '__main__':
    splitGroup(sys.argv[1], sys.argv[2])
