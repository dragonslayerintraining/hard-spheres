#!/usr/bin/python3

import sys
import time
import matplotlib.pyplot as plt

radius = 0.1
if len(sys.argv)>=2:
    radius = float(sys.argv[1])

plt.figure(figsize=(10,10))

for line in sys.stdin:
    arr=list(map(float,line.split(' ')))
    x=arr[0]
    y=arr[1]
    for sx in [-1,0,1]:
        for sy in [-1,0,1]:
            plt.gca().add_patch(plt.Circle((x+sx,y+sy),radius,color='b'))

plt.axis([0,1,0,1])
plt.xlabel("x")
plt.ylabel("y")
plt.title("Sample from hard disk model")

#plt.savefig("plot.png")
plt.plot()
plt.show()
