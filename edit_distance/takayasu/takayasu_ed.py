#Python3
# -*- coding: utf-8 -*-

import sys

def ins_cost(c):
	return 1
def sub_cost(c1,c2):
	if c1 == c2:
		return 0
	else:
		return 2
def del_cost(c):
	return 1

def edit_distance(source,target,mode=0):
	n = len(source)
	m = len(target)

	distance = [[-1000 for j in range(m+1)] for i in range(n+1)] #どうせ0以上になるのでわざわざいらない
	
	distance[0][0] = 0
	for i in range(1,n+1):
		distance[i][0] = distance[i-1][0]+del_cost(source[i-1])
	for j in range(1,m+1):
		distance[0][j] = distance[0][j-1]+ins_cost(target[j-1])
	
	for i in range(1,n+1):
		for j in range(1,m+1):
			distance[i][j] = min( distance[i-1][j  ]+del_cost(source[i-1]),
			                      distance[i-1][j-1]+sub_cost(source[i-1],target[j-1]),
			                      distance[i  ][j-1]+ins_cost(target[j-1]) )
	#表をプリント。デバッグ用
	if mode == 1:
		for l in range(n+1):
			print(distance[l])
	
	return distance[n][m]

"""
buff = input().split()
source = buff[0]
target = buff[1]
"""

if len(sys.argv) >=3:
	source = sys.argv[1]
	target = sys.argv[2]
else:
	source = "夜明け前より瑠璃色な"
	target = "けよりな"
print(source,target)

#デバッグ用
mode = 0
if len(sys.argv) >= 4:
	mode = int(sys.argv[3])


dist = edit_distance(source,target,mode)
print("dist =",dist)
