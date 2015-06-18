import sys
from numpy import *
import argparse
import keymap

ADD_REM_VALUE=1

def ed_op_table(string1,string2):
    distance = zeros((len(string1)+1,len(string2)+1))
    op_table = [[ "" for i in xrange(len(string2)+1)] for j in xrange(len(string1)+1)]
    for i in range(len(string1) + 1):
        for j in range(len(string2) + 1):
            if i == 0:
                distance[i][j] = j * ADD_REM_VALUE
                op_table[i][j] = '+'
            elif j == 0:
                distance[i][j] = i * ADD_REM_VALUE
                op_table[i][j] = '-'
            else:
                print keymap.key_distance2(string2[j-1],string2[j-2])
                rep = distance[i-1][j-1] if string1[i-1] == string2[j-1] else distance[i-1][j-1]+2*keymap.key_distance2(string1[i-1],string2[j-1])
                distance[i][j] = min (rep, distance[i-1][j]+ADD_REM_VALUE, distance[i][j-1]+keymap.key_distance2(string2[j-1],string2[j-2]))
                if distance[i][j] == rep:
                    op_table[i][j] = '|' if string1[i-1] == string2[j-1] else '*'
                elif distance[i][j] == distance[i-1][j]+ADD_REM_VALUE:
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

def main():
    print "please input String1 : \n>> ",
    string1=sys.stdin.readline().rstrip()
    print "please input String2 : \n>> ",
    string2=sys.stdin.readline().rstrip()
    print "Edit distance is",
    dist_table, op_table = ed_op_table(string1,string2)
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
    print op_table

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Calc Edit_distance between two strings')
    args = parser.parse_args()
    main()
