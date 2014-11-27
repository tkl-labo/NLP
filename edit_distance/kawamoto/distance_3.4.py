##!/usr/bin/env python
# -*- coding: utf-8 -*-

from sys import *
from numpy import *

ADD_REM_VALUE = 1
REPLACE_VALUE = 2

def edit_distance(string1,string2):
    distance = zeros((len(string1)+1,len(string2)+1))
    for i in range(len(string1) + 1):
        for j in range(len(string2) + 1):
            if i == 0:
                distance[i][j] = j
            elif j == 0:
                distance[i][j] = i
            else:
                if string1[i-1] == string2[j-1]:
                    distance[i][j] = min (distance[i-1][j-1], distance[i-1][j]+ADD_REM_VALUE, distance[i][j-1]+ADD_REM_VALUE)
                else:
                    distance[i][j] = min (distance[i][j-1]+REPLACE_VALUE, distance[i-1][j]+ADD_REM_VALUE, distance[i][j-1]+ADD_REM_VALUE)

    return distance[len(string1)][len(string2)]
                    
if __name__ == "__main__":
    print(edit_distance(argv[1],argv[2]))

