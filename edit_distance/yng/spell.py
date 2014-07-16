#!/usr/bin/env python
# -*- coding: utf-8 -*-
# spell.py -- a simple spell checker based on minmum edit distance
import sys

# cost functions
def ins_cost (c): return 1
def del_cost (c): return 1
def sub_cost (c1, c2): return 0 if c1 == c2 else 2

# return minimum edit distance for two UTF-8 encoded words
def min_edit_dist (target, source):
    n = len (target)
    m = len (source)
    dist = [[0  for j in range (m+1)] for i in range (n+1)]
    # initialize
    for i in range (1, n+1): dist[i][0] = dist[i-1][0] + ins_cost (target[i-1])
    for j in range (1, m+1): dist[0][j] = dist[0][j-1] + del_cost (source[j-1])
    # execute dynamic programming
    for i in range (1, n+1):
        for j in range (1, m+1):
            dist[i][j] = min (dist[i-1][j]   + ins_cost (target[i-1]),
                              dist[i-1][j-1] + sub_cost (target[i-1], source[j-1]),
                              dist[i][j-1]   + del_cost (source[j-1]))
    return dist[n][m]

words = [w[:-1].decode ('utf-8')
         for w in open (sys.argv[1] if len (sys.argv) >= 2
                        else "/usr/share/dict/words").readlines ()]

# loop
sys.stdout.write ("> ")
for line in iter (sys.stdin.readline, ""): # no buffering
    ws = [w.decode ('utf-8') for w in line[:-1].split (" ") if w]
    if len (ws) != 1:
        sys.stderr.write ("WARNING: print a single word after prompt '>'\n")
    else:
        for d, w in sorted ([[min_edit_dist (w, ws[0]), w] for w in words],
                            key=lambda x: x[0]):
            sys.stderr.write ("\t%s: %ld\n" % (w.encode ('utf-8'), d))
            sys.stdout.write ("-- stop enumeration? [y]: ")
            if sys.stdin.readline ()[:-1] == 'y': break

    sys.stdout.write ("> ")
