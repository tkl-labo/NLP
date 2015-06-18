##!/usr/bin/env python
# -*- coding: utf-8 -*-
from collections import defaultdict
from math import sqrt

keymap =[['1','2','3','4','5','6','7','8','9','0','-','^','\\'],
['q','w','e','r','t','y','u','i','o','p','@','['],
['a','s','d','f','g','h','j','k','l',';',':',']'],
['z','x','c','v','b','n','m',',','.','/']]

key_dict = defaultdict(tuple)
for i in xrange(len(keymap)):
    for j in xrange(len(keymap[i])):
        key_dict[keymap[i][j]]=(i,j)

key_dist_unit = 2 / sqrt((key_dict['z'][0]-key_dict['s'][0])**2+(key_dict['z'][1]-key_dict['s'][1])**2)

def key_distance(char1,char2):
#    return key_dist_unit * sqrt((key_dict[char1][0]-key_dict[char2][0])**2+(key_dict[char1][1]-key_dict[char2][1])**2)
    return 2

def key_distance2(char1,char2):
    return min(0.1+sqrt((key_dict[char1][0]-key_dict[char2][0])**2+(key_dict[char1][1]-key_dict[char2][1])**2)/2,1)


"""
print key_dict
print key_dict['z'], key_dict['\\']
"""
print key_dist_unit
