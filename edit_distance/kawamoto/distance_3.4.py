##!/usr/bin/env python
# -*- coding: utf-8 -*-

from sys import *
from numpy import *
string1 = argv[1]
string2 = argv[2]
distance = zeros((len(string1)+1,len(string2)+1))
for i in range(len(string1) + 1):
    for j in range(len(string2) + 1):
        if i == 0:
            distance[i][j] = j
        elif j == 0:
            distance[i][j] = i
        else:
            if string1[i-1] == string2[j-1]:
                distance[i][j] = min (distance[i-1][j-1], distance[i-1][j]+1, distance[i][j-1]+1)
            else:
                distance[i][j] = min (distance[i][j-1]+2, distance[i-1][j]+1, distance[i][j-1]+1)

print(distance[len(string1)][len(string2)])

