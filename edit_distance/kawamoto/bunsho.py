##!/usr/bin/env python
# -*- coding: utf-8 -*-
import distance
import kanji
from sys import *
from subprocess import Popen, PIPE
import itertools
cmdline = "echo \"" + argv[1] + "\" | kakasi -s -i utf-8 -o utf-8"
p=Popen(cmdline, shell=True, stdin=PIPE, stdout=PIPE, close_fds=True, universal_newlines=True)
sent1 = (p.stdout.readline()).rstrip().split()
cmdline = "echo \"" + argv[2] + "\" | kakasi -s -i utf-8 -o utf-8"
p=Popen(cmdline, shell=True, stdin=PIPE, stdout=PIPE, close_fds=True, universal_newlines=True)
sent2 = (p.stdout.readline()).rstrip().split()
cost =[]
if len(sent1) > len(sent2):
    long = sent1
    short = sent2
else:
    long = sent2
    short = sent1
tmp = short[:]
for i in range(len(long)-len(short)):
    tmp.append("")
comb = list(itertools.combinations(long, len(short)))
print (comb)
for i in range(len(comb)):
    cost.append(0)
    for j in range(len(short)):
        cost[i] += kanji.yomi_distance(short[j],comb[i][j])
    print(cost[i])


#if __name__ == "__main__":
#    print(yomi_distance(argv[1],argv[2]))
          
