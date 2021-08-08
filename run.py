import os 
import subprocess
import shutil
import time

# replace Data2 by Data2 and Output2 by Output2

ll=os.listdir("Data2")
time_lst=[]
len_lst=[]
name_lst=[]

flag=0


for dirr in ll:
	print("dirr: ",dirr)
	if dirr in os.listdir("Output2"):
		continue

	dirr2=dirr
	dirr2=dirr2[13:]
	ind=dirr2.find("_")
	count=dirr2[:ind]	

	name_lst.append(dirr)

	tm=0.0
	le=0.0

	os.mkdir("Output2/"+dirr)	

	for f in os.listdir("Data2/"+dirr):
		#print("f: ",f)
		# x=open("input","w")
		# y=open("Data2/"+dirr+"/"+f,"r")
		# lines=y.readlines()
		# for line in lines:
		# 	x.write(line)
		if f in os.listdir("Output2/"+dirr):
			continue

		dirr2=dirr[13:]
		ind=dirr2.find("_")
		count=dirr2[:ind]

		subprocess.call("python3 straighten.py Data2/"+dirr+"/"+f,shell=True)

	
		#print("1")
		#subprocess.run(["./a.out","<","input"])
		subprocess.call("./a.out < input",shell=True)
		


		with open("Output2/"+dirr+"/"+f+"_Logs","w") as g:
			l=""
			with open("Time","r") as t:
				l=t.read()
			g.write(l+"\n")
			tm+=float(l)
			
			with open("Len","r") as t:
				l=t.read()
			g.write(l+"\n")
			le+=float(l)

		

	tm=tm/300
	le=le/300

	time_lst.append(tm)
	len_lst.append(le)
	print("time: ",tm)
	print("tree_length: ",le)
	print("")

	with open("fin2.txt","a") as fp:
		fp.write("dirr: "+dirr+"\n")
		fp.write("time: "+str(tm)+"\n")
		fp.write("tree_length: "+str(le)+"\n")
		fp.write("\n")

print(name_lst)
print(time_lst)
print(len_lst)

with open("avg_time","w") as fg:
	for nm in name_lst:
		fg.write(str(nm)+" ")
	fg.write("\n")	
	for nm in time_lst:
		fg.write(str(nm)+" ")
	fg.write("\n")	

with open("avg_len_of_tree","w") as fg:
	for nm in name_lst:
		fg.write(nm+" ")
	fg.write("\n")	
	for nm in len_lst:
		fg.write(nm+" ")
	fg.write("\n")		

			
