#!/usr/bin/env python
# cky.py: parse given input using CKY algorithm
import sys, collections, itertools

if len (sys.argv) != 2:
    sys.exit ("Usage: %s rules" % sys.argv[0])

rs2l = collections.defaultdict (list)

def cky_parse (sentence):
    n = len (sentence)
    cky_table = [[collections.defaultdict (list) for j in range (n + 1)]
                 for i in range (n + 1)]
    # lookup lexicon
    for i in range (n):
        for lhs in rs2l[tuple (["_" + sentence[i]])]:
            cky_table[i][i + 1][lhs].append (sentence[i])
    # parse
    for i in range (2, n + 1):
        for j in reversed (range (i + 1)):
            for k in range (j, i):
                for rhs in itertools.product (cky_table[j][k], cky_table[k][i]):
                    for lhs in rs2l[rhs]:
                        cky_table[j][i][lhs].append ([[j,k,i], rhs])
    return cky_table

def num_edges (cky_table):
    return sum (len (cky_table[j][i])
                for i in range (len (cky_table))
                for j in range (i + 1))

def num_trees (cky_table, label = 'S', beg = 0, end = 0):
    if end - beg == 1: # terminal
        return 1 if label in cky_table[beg][end] else 0
    return sum (num_trees (cky_table, x, j, k) *
                num_trees (cky_table, y, k, i)
                for (j, k, i), (x, y) in cky_table[beg][end][label])

def recover_trees (cky_table, label = 'S', beg = 0, end = 0):
    if end - beg == 1: # terminal
        yield [label, cky_table[beg][end][label][0]]
    else:
        for (j, k, i), (x, y) in cky_table[beg][end][label]:
            for l, r in itertools.product (recover_trees (cky_table, x, j, k),
                                           recover_trees (cky_table, y, k, i)):
                yield [label, l, r]

def treefy (tree, l = 0):
    ret = "%s(%s" % ("  " * l, tree[0])
    rhs = tree[1:]
    if len (rhs) == 1: # terminal
        ret += " %s)" % rhs[0]
    else:
        ret += "\n%s\n%s)" % (treefy (rhs[0], l + 1), treefy (rhs[1], l + 1))
    return ret

# read rules
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
    elif command == 'stat':
        if sentence:
            print "sentence: %s"     % ' '.join (sentence)
            print "length:   %d"     % len (sentence)
            print "# edges:  %d"     % num_edges (cky_table)
            print "# trees:  %d" % num_trees (cky_table, 'S', 0, len (sentence))
    elif command == 'print':
        if sentence:
            i = 1
            for tree in recover_trees (cky_table, 'S', 0, len (sentence)):
                # print_tree (revert_rule (tree))
                print "tree #%s\n%s\n" % (i, treefy (tree))
                i += 1
    else:
        sys.stderr.write ("unknown command: %s\n" % command)
    sys.stdout.write ("> ")
