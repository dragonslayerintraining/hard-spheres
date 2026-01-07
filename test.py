#!/usr/bin/python3

import sys
import time
import matplotlib.pyplot as plt


plt.figure(figsize=(10,10))

for line in sys.stdin:
    arr=list(map(float,line.split(' ')))
    x=arr[0]
    y=arr[1]
    for sx in [-1,0,1]:
        for sy in [-1,0,1]:
            plt.gca().add_patch(plt.Circle((x+sx,y+sy),0.1,color='b'))

plt.axis([0,1,0,1])
plt.xlabel("x")
plt.ylabel("y")
plt.title("Points")

plt.plot("plot.png")
plt.show()
