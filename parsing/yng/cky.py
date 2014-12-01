#!/usr/bin/env python
# cky.py: parse given input using CKY algorithm
import sys, collections, itertools

if len (sys.argv) != 2:
    sys.exit ("Usage: %s rules" % sys.argv[0])

rs2l = collections.defaultdict (list)

def cky_parse (sentence):
    n = len (sentence)
    cky_table = [[set () for j in range (n + 1)] for i in range (n + 1)]
    # lookup lexicon
    for i in range (n):
        for lhs in rs2l[tuple (["_" + sentence[i]])]:
            cky_table[i][i + 1].add (lhs)
        if len (cky_table[i][i + 1]) == 0:
            sys.stderr.write ("unknown word: %s\n" % sentence[i])
            return cky_table
    # parse
    for i in range (2, n + 1):
        for j in reversed (range (i + 1)):
            for k in range (j, i):
                for rhs in itertools.product (cky_table[j][k], cky_table[k][i]):
                    for lhs in rs2l[rhs]:
                        cky_table[j][i].add (lhs)
    return cky_table

def num_edges (cky_table):
    return sum (len (cky_table[j][i])
                for i in range (len (cky_table))
                for j in range (i + 1))

for line in open (sys.argv[1]):
    rule = line[:-1].split (" ")
    lhs  = rule[0]
    rhs  = rule[1:]
    rs2l[tuple (rhs)].append (lhs)

# loop
sentence  = ""
cky_table = []
sys.stdout.write ("> ")
for line in iter (sys.stdin.readline, ""): # no buffering
    command = line[:-1].split (" ", 1)[0]
    if command == 'parse':
        sentence  = line[:-1].split (" ", 1)[1][1:-1].split (" ")
        cky_table = cky_parse (sentence)
        print "success" if 'S' in cky_table[0][len (sentence)] else 'fail'
    else:
        if sentence:
            if command == 'show_stat':
                print "sentence:  %s"      % ' '.join (sentence)
                print "length:    %d"      % len (sentence)
                print "# edges:   %d"      % num_edges (cky_table)
#            print "# parse trees: %d\n" % num_parse_trees ())
            elif command == 'show_trees':
#                print_trees ()
                pass
    sys.stdout.write ("> ")
