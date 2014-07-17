##!/usr/bin/env python
# -*- coding: utf-8 -*-
import distance
from sys import *
from subprocess import Popen, PIPE
def yomi_distance(input_a,input_b):
    cmdline = "echo \"" + input_a + "\" | kakasi -JH -KH -EH -KH -i utf-8 -o utf-8"
    p=Popen(cmdline, shell=True, stdin=PIPE, stdout=PIPE, close_fds=True, universal_newlines=True)
    string1 = (p.stdout.readline()).rstrip()
    print( "input string1 ", string1)
    cmdline = "echo \"" + input_b + "\" | kakasi -JH -KH -EH -KH -i utf-8 -o utf-8"
    p=Popen(cmdline, shell=True, stdin=PIPE, stdout=PIPE, close_fds=True, universal_newlines=True)
    string2 = (p.stdout.readline()).rstrip()
    print( "input string2 ", string2)
    return distance.edit_distance(string1,string2)


if __name__ == "__main__":
    print(yomi_distance(argv[1],argv[2]))
          
