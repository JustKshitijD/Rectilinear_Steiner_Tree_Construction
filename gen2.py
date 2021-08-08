#rm -r no_of_points_*
import random
import numpy as np
import os
import time
import shutil
import sys

no_of_points=[]
k=0
no_of_points.append(5100)
# for i in range(10,15):
#     no_of_points.appecnd(i)

gaussian_noise=[]
gaussian_noise.append(7)

xx=[]
yy=[]
xx2=[]
yy2=[]


#shutil.rmtree("Data")
#os.mkdir("Data")
os.chdir("./Data")

st=time.time()

c=0
dirr=os.listdir()

for i in range(0,len(no_of_points)):
	# xx=[]
	# yy=[]
	# xx2=[]
	# yy2=[]
	print("########################################")  
	for g in range(0,len(gaussian_noise)):
		no_p=no_of_points[i]
		sigma=gaussian_noise[g]

		dir_name="no_of_points_"+str(no_p)+"_sigma_"+str(sigma)
		if dir_name in dirr:
			continue

		if c!=0:
			os.chdir("../")
		c+=1

		os.mkdir(dir_name)
		os.chdir(dir_name)

		print("no_p: ",no_p)
		print("sigma: ",sigma)
		print("")

		# file_name_x="no_of_points_"+str(no_p)+"_sigma_"+str(sigma)+"_x"
		# file_name_y="no_of_points_"+str(no_p)+"_sigma_"+str(sigma)+"_y"

		for j in range(0,500):
			#print("j: ",j)
			# choices_x=[]
			# choices_y=[]
			# for k in range(0,no_p):
			#     choices_x.append(k+1)
			#     choices_y.append(k+1)
			# random.shuffle(choices_x)
			# random.shuffle(choices_y)

			# x=[]
			# y=[]
			# while choices_x:
			#     x.append(choices_x.pop())
			#     y.append(choices_y.pop())
			x = np.random.uniform(0,3*no_p,no_p)
			y = np.random.uniform(0,3*no_p,no_p)
			#print("x: ",x)
			#print("y: ",y)

			if sigma!=0:
				# if ((list(x) in xx) and (list(y) in yy)) :
				#     print("NO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
				#     j=j-1
				#     continue

				xx.append(list(x))
				yy.append(list(y))

				os.chdir("../no_of_points_"+str(no_p)+"_sigma_0")

				with open("no_of_points_"+str(no_p)+"_sigma_0_"+str(j),"r") as fp:
					lines=fp.readlines()
					x=lines[0]
					y=lines[1]

				os.chdir("../"+dir_name)	

				x=x.split()
				y=y.split()

				#print("x: ",x)
				#print("y: ",y)
				#print("")

				#print(type(x))
				g1=[]
				g2=[]

				for d in x:
					#print("d: ",d)
					g1.append(float(d))
				for d in y:
					g2.append(float(d))	

				x=np.array(g1)
				y=np.array(g2)

				#print("x: ",x)
				#print("y: ",y)
				#print("")

				#print("x.shape: ",x.shape)

				noise = np.random.normal(0,sigma,x.shape)
				#print("noise: ",noise)

				new_x = x + noise
				new_y = y + noise

				#print("new_x: ",new_x)
				#print("new_y: ",new_y)
				#print("len(new_x): ",len(new_x))
				#print("len(new_y): ",len(new_y))

				# if ((list(new_x) in xx2) and (list(new_y) in yy2)) :
				#     print("NOOOOOO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
				#     j=j-1
				#     continue
			else:
				new_x=x
				#print("new_x: ",new_x)
				new_y=y        

			#print("newwww_x: ",new_x)	
			# xx2.append(list(new_x))
			# yy2.append(list(new_y))

			x_l=list(new_x)
			y_l=list(new_y)

			file_name=dir_name+"_"+str(j)

			with open(file_name,'a') as fp:
			    for r in range(0,len(x_l)):
			        fp.write(str(x_l[r])+" ")
			    fp.write("\n")
			    for r in range(0,len(y_l)):
			        fp.write(str(y_l[r])+" ")
			    fp.write("\n")

                

en=time.time()

print("Time spent: ",en-st)                
