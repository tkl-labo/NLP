#!/usr/bin/env python
# -*- coding: utf-8 -*-

from sys import *
from collections import deque
from collections import defaultdict

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

def append_dict(*args, **kwargs):
    dict = args[0]
    dict

if __name__ == "__main__":
    if len(argv) != 2:
        print("Usage: # python %s \"N\" " % argv[0])
        quit()
    N = argv[1]
    queue = deque('^')
    dict = multi_dimention_dict(N)
    for line in stdin:
        word = line.split()[0]
        if len(queue) == N:
            dict[queue] = dict.get(queue,0) + 1
            queue.popleft()
            queue.append(word)
        else:
            queue.append(word)
    dict[queue] = dict.get(queue,0) + 1
