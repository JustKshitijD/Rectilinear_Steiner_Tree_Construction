import os
l=os.listdir("Data")
for i in range(0,len(l)):
	ll=os.listdir("Data/"+l[i])

	print("l[i]: ",l[i],":- ",len(ll))