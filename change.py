import os
import shutil

os.chdir("Data")
for dirr in os.listdir("."):
	if dirr!="no_of_points_4100_sigma_1":
		continue
		
	os.chdir(dirr)
	print(dirr)
	# dirr2=dirr
	# dirr=dirr[13:]
	# ind=dirr.find("_")
	# count=dirr[:ind]
	# print(count)
	

	for ff in os.listdir("."):
		ll=[]
		#print("ff: ",ff)
		with open(ff,"r") as fptr:
			ll=fptr.readlines()	
	
		#if(len(ll)==2):
		#print(len(ll))
		#print("h")
		with open(ff+"_2","w") as fptr:
			#fptr.write(count+"\n")
			cc=0

			for line in ll:
				# if cc>=3:
				if cc==0:
					fptr.write(line)
				else:	
					lst=line.split()
					#print(lst)
					for i in range(0,len(lst)):
						lst[i]=int(float(lst[i]))
					for i in lst:
						fptr.write(str(i)+" ")
					fptr.write("\n")	
				cc+=1

		os.remove(ff)
		os.rename("./"+ff+"_2","./"+ff)


	os.chdir("../")			


