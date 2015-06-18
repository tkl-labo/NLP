##!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from numpy import *
import argparse

def edit_distance(string1,string2):
    distance = zeros((len(string1)+1,len(string2)+1))
    for i in range(len(string1) + 1):
        for j in range(len(string2) + 1):
            if i == 0:
                distance[i][j] = j * ADD_REM_VALUE
            elif j == 0:
                distance[i][j] = i * ADD_REM_VALUE
            else:
                if string1[i-1] == string2[j-1]:
                    distance[i][j] = distance[i-1][j-1]
                else:
                    distance[i][j] = min (distance[i-1][j-1]+REPLACE_VALUE, distance[i-1][j]+ADD_REM_VALUE, distance[i][j-1]+ADD_REM_VALUE)
    return distance

def ed_op_table(string1,string2, add_rem_value, rep_value):
    distance = zeros((len(string1)+1,len(string2)+1))
    op_table = [[ "" for i in xrange(len(string2)+1)] for j in xrange(len(string1)+1)]
    for i in range(len(string1) + 1):
        for j in range(len(string2) + 1):
            if i == 0:
                distance[i][j] = j * add_rem_value
                op_table[i][j] = '+'
            elif j == 0:
                distance[i][j] = i * add_rem_value
                op_table[i][j] = '-'
            else:
                if string1[i-1] == string2[j-1]:
                    distance[i][j] = distance[i-1][j-1]
                    op_table[i][j] = '|'
                else:
                    distance[i][j] = min (distance[i-1][j-1]+rep_value, distance[i-1][j]+add_rem_value, distance[i][j-1]+add_rem_value)
                    if distance[i][j] == distance[i-1][j-1]+rep_value:
                        op_table[i][j] = '*'
                    elif distance[i][j] == distance[i-1][j]+add_rem_value:
                        op_table[i][j] = '-'
                    else:
                        op_table[i][j] = '+'
    return distance, op_table

def back_trace(op_table):
    i, j =len(op_table)-1, len(op_table[0])-1
    while i > 0 or j > 0:
        yield op_table[i][j]
        if op_table[i][j] == '|' or op_table[i][j]== '*':
            i-=1
            j-=1
        elif op_table[i][j] == '-':
            i-=1
        else:
            j-=1

def parse_op_table(op_table):
    operations = [i for i in back_trace(op_table)]
    operations.reverse()
    return operations

def main(add_rem_value, rep_value):
    print "please input String1 : \n>> ",
    string1=sys.stdin.readline().rstrip()
    print "please input String2 : \n>> ",
    string2=sys.stdin.readline().rstrip()
    print "Edit distance is",
    dist_table, op_table = ed_op_table(string1,string2,add_rem_value, rep_value)
    print dist_table[len(dist_table)-1, len(dist_table[0])-1]
    print "Operations are below"
    operations = parse_op_table(op_table)
    string1_ = list(string1)
    string2_ = list(string2)
    for i in xrange(len(operations)):
        if operations[i] == '-':
            string2_.insert(i,' ')
        elif operations[i] == '+':
            string1_.insert(i,' ')
    print list(string1_)
    print parse_op_table(op_table)
    print list(string2_)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Calc Edit_distance between two strings')
    parser.add_argument('-a', '--add',type=int, dest='add_rem_value', nargs='?',\
     default=1, help='add_rem_value (default=1)')
    parser.add_argument('-r', '--rep',type=float, dest='rep', nargs='?',\
     default=2, help='replace_value (default=2)')
    args = parser.parse_args()
    main(args.add_rem_value,args.rep)
