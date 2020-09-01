import re
import sys

if (len(sys.argv)<2):
    print ('Usage: updateLibraryVersion.sh version')
    exit()

#input file
fin = open("library.properties", "r")
fout = open("out.txt", "w")

for line in fin:
    st = re.search('^version=',line)
    if (st!=None):
        fout.write('version='+str(sys.argv[1])+'\n')
    else:
        fout.write(line)	
fin.close()
fout.close()

fin = open("library.json", "r")
fout = open("out2.txt", "w")

for line in fin:
    st = re.search('"version":',line)
    if (st!=None):
        fout.write('    "version": "'+str(sys.argv[1])+'",\n')
    else:
        fout.write(line)	
fin.close()
fout.close()
