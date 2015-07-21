#!/usr/bin/env python

#Data file name
f="MetDept1.csv"

#Read CSV files
import csv

# open csv file
csvfile = open( f, "rb" )

# sniff into 10KB of the file to check its dialect
dialect = csv.Sniffer().sniff( csvfile.read( 10*1024 ) )
csvfile.seek(0)

# read csv file according to dialect
reader = csv.reader( csvfile, dialect )

# read header
header = reader.next()

# read data into memory
data = [row for row in reader]

# close input file
csvfile.close()

print(header)
print(data[-2][:])

#Plot some data
import datetime
import matplotlib.dates as dlt
X=[]
for i in range(len(data)):
	if(i%1000 == 0):
		print("Timestamp",i,len(data))
	#5 is GMTTime
	if(data[:][i][4] != '' and data[:][i][5] != ''):
		day,month,year=data[:][i][4].split("/")
		hour,minute,second=data[:][i][5].split(":")
		try:
			X.append(dlt.date2num(datetime.datetime(int(year), int(month), int(day), int(hour), int(minute), int(second))))
		except:
			X.append(0.0)
	else:
		X.append(0.0)

print(header[16])
Y=[]
for i in range(len(data)):
	if(i%1000 == 0):
		print("Data",i,len(data))
	#16 is rain hourly I think
	if(data[:][i][16] != ''):
		try:
			Y.append(float(data[:][i][16]))
		except:
			Y.append(0.0)
	else:
		Y.append(0.0)

print('graphing rain hourly')
import numpy as np
import matplotlib.pyplot as plt
fig = plt.figure(figsize=(23.5, 13.0)) 
plt.title("Accumulation of hourly rain (mm/h)")
plt.ylabel("Rainfall (mm/h)")
plt.xlabel("Time (days)")
plt.plot(X[:-2],Y[:-2])
fig.savefig('MWSDataAnalysis.png', facecolor=fig.get_facecolor(), transparent=True)
fig.show()

