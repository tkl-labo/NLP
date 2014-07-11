#!/usr/bin/env python
# -*- coding: utf-8 -*-
# align.py -- make alignment for given two words
import sys, unicodedata

# cost functions
def ins_cost (c): return 1
def del_cost (c): return 1
def sub_cost (c1, c2): return 0 if c1 == c2 else 2

# return minimum edit distance and back pointers for two UTF-8 encoded words
def min_edit_dist (target, source):
    n = len (target)
    m = len (source)
    dist = [[0]  * (m+1) for i in range (n+1)]
    bptr = [[""] * (m+1) for i in range (n+1)]
    # initialize
    for i in range (1, n+1): dist[i][0] = dist[i-1][0] + ins_cost (target[i-1])
    for j in range (1, m+1): dist[0][j] = dist[0][j-1] + del_cost (source[j-1])
    for i in range (1, n+1): bptr[i][0] = ['i']
    for j in range (1, m+1): bptr[0][j] = ['d']
    # execute dynamic programming
    for i in range (1, n + 1):
        for j in range (1, m + 1):
            dist_i = dist[i-1][j]   + ins_cost (target[i-1])
            dist_s = dist[i-1][j-1] + sub_cost (target[i-1], source[j-1])
            dist_d = dist[i][j - 1] + del_cost (source[j-1])
            dist[i][j] = min (dist_i, dist_s, dist_d)
            if dist[i][j] == dist_i: bptr[i][j] += 'i'
            if dist[i][j] == dist_s: bptr[i][j] += 's'
            if dist[i][j] == dist_d: bptr[i][j] += 'd'
    return dist[n][m], bptr

def alignment_path (n, m, bptr):
    if n == m == 0: yield ""
    for action in bptr[n][m]:
        for path in alignment_path (n if action == 'd' else n-1,
                                    m if action == 'i' else m-1,
                                    bptr):
            yield path + action

def width (c):
    return 2 if unicodedata.east_asian_width (c) in u"WFA" else 1

def print_alignment (source, target, path):
    i, j, path_aligned, source_aligned, target_aligned = 0, 0, "", "", ""
    for action in path:
        path_aligned   += action + " " * (width (target[j] if action == 'd' else source[i]) - 1)
        source_aligned += source[i] if action != 'd' else " " * width (target[j])
        target_aligned += target[j] if action != 'i' else " " * width (source[i])
        if action != 'd': i += 1
        if action != 'i': j += 1
    sys.stdout.write ("\taction:\t%s\n\tsource:\t%s\n\ttarget:\t%s\n"
                      % (path_aligned,
                         source_aligned.encode ('utf-8'),
                         target_aligned.encode ('utf-8')))

# loop
sys.stdout.write ("> ")
for line in iter (sys.stdin.readline, ""): # no buffering
    ws = [w for w in line[:-1].decode ('utf-8').split (" ") if w]
    if len (ws) == 2:
        d, bptr = min_edit_dist (ws[0], ws[1])
        sys.stderr.write ("dist ('%s', '%s') = %d\n" %
                          (ws[0].encode ('utf-8'), ws[1].encode ('utf-8'), d))
        sys.stderr.write ("alignment:\n")
        for path in alignment_path (len (ws[0]), len (ws[1]), bptr):
            print_alignment (ws[0], ws[1], path)
            sys.stdout.write ("-- stop enumeration? [y]: ")
            if sys.stdin.readline ()[:-1] == 'y': break
    else:
        sys.stderr.write ("Usage:\n")
        sys.stderr.write ("\t> word1 word2 # show alignment\n")
    sys.stdout.write ("> ")
