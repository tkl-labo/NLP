#!/usr/bin/env python
# -*- coding:utf-8 -*-
import sys,os
#import math,re,datetime

##usage
##python edit_distance.py target_string source_string

def edit_distance(target,source):
    target_u = target.decode("utf8")
    source_u = source.decode("utf8")

    n = len(target_u)
    m = len(source_u)

    distance_matrix = [ [None for col in range(m+1)] for row in range(n+1)]
    #initialize
    for i in range(n+1):
        distance_matrix[i][0] = i
    for j in range(m+1):
        distance_matrix[0][j] = j
    
    for i in range(1,n+1):
        for j in range(1,m+1):
            ins_cost = distance_matrix[i-1][j] + 1
            del_cost = distance_matrix[i][j-1] + 1
            if target_u[i-1] == source_u[j-1]:
                subst_cost = distance_matrix[i-1][j-1]
            else:
                subst_cost = distance_matrix[i-1][j-1] + 2
            distance_matrix[i][j] = min([ins_cost,del_cost,subst_cost])
        
    return distance_matrix[n][m]


if __name__ == "__main__":
    target = sys.argv[1]
    source = sys.argv[2]

    print edit_distance(target,source)
