##!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from numpy import *
import argparse

ADD_REM_VALUE = 1
REPLACE_VALUE = 1.5

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

def ed_op_table(string1,string2):
    distance = zeros((len(string1)+1,len(string2)+1))
    op_table = [[ "" for i in xrange(len(string1)+1)] for j in xrange(len(string2)+1)]
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
    return distance, op_table

def main():
    """
    print "please input String1 : \n>> ",
    string1=sys.stdin.readline().rstrip()
    print "please input String2 : \n>> ",
    string2=sys.stdin.readline().rstrip()
    """
#    print "Edit distance is",
    string1="aba"
    string2="abb"
    distance_table = ed_op_table(string1,string2)
    print distance_table

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Calc Edit_distance between two strings')
    parser.add_argument('-p', '--per',type=int, dest='per', nargs='?',\
     default=30, help='period_days (default=30)')
    args = parser.parse_args()
    main()
