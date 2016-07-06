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

	distance = [[-1000 for j in range(m+1)] for i in range(n+1)]
	backpointer = [[[0 for k in range(3)] for j in range(m+1)] for i in range(n+1)]

	distance[0][0] = 0
	for i in range(1,n+1):
		distance[i][0] = distance[i-1][0]+del_cost(source[i-1])
		backpointer[i][0][0] = 1
	for j in range(1,m+1):
		distance[0][j] = distance[0][j-1]+ins_cost(target[j-1])
		backpointer[0][j][2] = 1
	for i in range(1,n+1):
		for j in range(1,m+1): 
			a = [ distance[i-1][j  ]+del_cost(source[i-1]),
			      distance[i-1][j-1]+sub_cost(source[i-1],target[j-1]),
			      distance[i  ][j-1]+ins_cost(target[j-1]) ]
			distance[i][j] = min(a)
			for k in range(3):
				if a[k] == distance[i][j]:
					backpointer[i][j][k] = 1
	#表をプリント。デバッグ用
	if mode == 1:
		for l in range(n+1):
			print (distance[l])
		print("[1,1,1] means ↑＼←")
		for l in range(n+1):
			print(backpointer[l])
	
	return distance[n][m], backpointer

def backtrace_sub(i,j,actions):
	if i==0 and j==0:
		global actions_list
		actions_list.append(actions) #plint
		return None
	if backpointer[i][j][0] == 1:
		backtrace_sub(i-1,j,['del']+actions)
	if backpointer[i][j][1] == 1:
		backtrace_sub(i-1,j-1,['sub']+actions)
	if backpointer[i][j][2] == 1:
		backtrace_sub(i,j-1,['ins']+actions)
	
def backtrace(source, target):
	n = len(source)
	m = len(target)
	backtrace_sub(n,m,[])
	return None
	
	
if len(sys.argv) >=3:
	source = sys.argv[1]
	target = sys.argv[2]
else:
	source = "夜明け前より瑠璃色な"
	target = "けよりな"
print (source,target)

#デバッグ用
mode = 0
if len(sys.argv) >= 4:
	mode = int(sys.argv[3])

dist, backpointer = edit_distance(source,target,mode)
actions_list = []
backtrace(source,target)
print("dist =",dist)
print("number of kinds of actions =", len(actions_list))


#print(actions_list)

for i in range(len(actions_list)):
	if i >= 5:
		break
	align_action=""
	align_source=""
	align_target=""
	index_source = 0
	index_target = 0
	flag_japanese = 0
	if len(bytes(source,'utf-8')) != len(source):
		flag_japanese = 1
	chars=[['-','=','*','+',' '],["－","＝","×","＋","　"]]
	for j in range(len(actions_list[i])):
		if actions_list[i][j] == 'del':
			align_action += chars[flag_japanese][0]# "-"
			align_source += source[index_source]
			index_source += 1
			align_target += chars[flag_japanese][4]# " "
		if actions_list[i][j] == 'sub':
			if source[index_source] == target[index_target]:
				align_action += chars[flag_japanese][1]# "="
			else:
				align_action += chars[flag_japanese][2]# "*"
			align_source += source[index_source]
			index_source += 1
			align_target += target[index_target]
			index_target += 1
		if actions_list[i][j] == 'ins':
			align_action += chars[flag_japanese][3]# "+"
			align_source += chars[flag_japanese][4]# " "
			align_target += target[index_target]
			index_target += 1
			
	print(align_action)
	print(align_source)
	print(align_target)
	
