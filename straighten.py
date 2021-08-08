import os
import sys

#print("1")
with open(sys.argv[1],"r") as fp:
	l=fp.readlines()
	c=l[0]
	l1=l[1]
	l2=l[2]
	l1=l1.split()
	l2=l2.split()

	with open("input","w") as f:
		f.write(c)
		for i in range(0,len(l1)):
			f.write(str(l1[i])+" ")
			f.write(str(l2[i]))
			f.write("\n")