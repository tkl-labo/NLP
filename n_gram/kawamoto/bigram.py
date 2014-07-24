#!/usr/bin/env python
# -*- coding: utf-8 -*-

from sys import *
from collections import deque
from collections import defaultdict
import codecs

#可変長引数
def ngram(*args, **kwargs):
    print (len(args))
    
def multi_dimension_dict(dimension, callable_obj=int):
    nodes = defaultdict(callable_obj)
    for i in range(dimension-1):
        p = nodes.copy()
        nodes = defaultdict(lambda : defaultdict(p.default_factory))
    return nodes
# http://materia.jp/blog/20121119.html

if __name__ == "__main__":
#    if len(argv) != 2:
#        print("Usage: # python %s \"N\" " % argv[0])
#        quit()
    N = 2
    queue = deque()
    dict = multi_dimension_dict(2)
    i = 0
    stdin = codecs.getreader('utf8')(stdin.detach(), errors='ignore')
    for line in stdin:
        print ("Now line : %d processing!!" % i)
        i += 1
        try:
            word = line.split()[0]
            if len(queue) == N:
                if word == "EOS":
                    dict[queue[0]][queue[1]] += 1
                    queue.popleft()
                    queue.append(word)
                    dict[queue[0]][queue[1]] += 1
                    queue.clear()
                else:
                    dict[queue[0]][queue[1]] += 1
                    queue.popleft()
                    queue.append(word)
            else:
                queue.append(word)
        except IndexError:
            print ("IndexError")

#    dict[queue[0]][queue[1]] += 1
    print (dict)
    for k1, dict2 in dict.items():
        print (k1,dict2)
        for k2, v in dict2.items():
            print (k1,k2,v)

