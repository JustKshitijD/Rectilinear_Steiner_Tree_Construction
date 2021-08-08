import os
import shutil
import subprocess

l=os.listdir("Data")
l1=[]
l2=[]
# l3=[]
# l4=[]
print(l)

for i in range(0,60):
	print(i)
	if(i<30):
		l1.append(l[i])
	elif i<60:
		l2.append(l[i])
	# elif i<90:
	# 	l3.append(l[i])	
	# elif i<120:
	# 	l4.append(l[i])


os.mkdir("Data1")
for f in l1:
	subprocess.call("cp -r Data/"+f+" Data1/"+f,shell=True)	


os.mkdir("Data2")
for f in l2:
	subprocess.call("cp -r Data/"+f+" Data2/"+f,shell=True)	
		

# os.mkdir("Data3")
# for f in l3:
# 	subprocess.call("cp -r Data/"+f+" Data3/"+f,shell=True)	
		

# os.mkdir("Data4")
# for f in l4:
# 	subprocess.call("cp -r Data/"+f+" Data4/"+f,shell=True)	

