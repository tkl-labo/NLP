#!/usr/bin/env python
# cky.py: parse given input using CKY algorithm
import sys, collections, itertools

if len (sys.argv) != 2:
    sys.exit ("Usage: %s grammar" % sys.argv[0])

grammar = collections.defaultdict (list) # mapping from rhs to lhs

def parse (words):
    n = len (words)
    table = [[collections.defaultdict (list) for j in range (n + 1)]
             for i in range (n + 1)]
    # parse
    for j in range (1, n + 1):
        for lhs in grammar[("_" + words[j - 1], )]:
            table[j - 1][j][lhs].append (words[j - 1]) # lookup lexicon
        for i in reversed (range (j)):
            for k in range (i + 1, j):
                for rhs in itertools.product (table[i][k], table[k][j]):
                    for lhs in grammar[rhs]:
                        table[i][j][lhs].append ([k, rhs])
    return table

def num_edges (table):
    return sum (sum (len (edges) for edges in t) for t in table)

def num_trees (table, label, i, j):
    if j - i == 1: # terminal
        return 1 if label in table[i][j] else 0
    return sum (num_trees (table, x, i, k) * num_trees (table, y, k, j)
                for k, (x, y) in table[i][j][label])

def recover_trees (table, label, i, j):
    if j - i == 1: # terminal
        yield [label, table[i][j][label][0]]
    else:
        for k, (x, y) in table[i][j][label]:
            for l, r in itertools.product (recover_trees (table, x, i, k),
                                           recover_trees (table, y, k, j)):
                yield [label, l, r]

def treefy (tree, l = 0):
    if tree[0][0] == 'X': # reduce rules introduced in converting into CNF
        return "\n".join (treefy (c, l) for c in tree[1:])
    lhs, rhs = tree[0], tree[1:]
    return "%s(%s %s)" % ("  " * l, lhs,
                          rhs[0] if len (rhs) == 1 else
                          '\n'.join ([""] + [treefy (r, l + 1) for r in rhs]))

# read grammar
for line in open (sys.argv[1]):
    rule = line[:-1].split (" ")[:-1] # delete prob
    lhs, rhs = rule[0], rule[1:]
    grammar[tuple (rhs)].append (lhs)

# loop
words = []
table = []
sys.stdout.write ("> ")
for line in iter (sys.stdin.readline, ""): # no buffering
    command = line[:-1].split (" ", 1)[0]
    if command == 'parse':
        words = line[:-1].split (" ", 1)[1][1:-1].split (" ")
        table = parse (words)
        print "success" if 'S' in table[0][len (words)] else 'fail'
    elif command == 'stat':
        if words:
            print "sentence: %s" % ' '.join (words)
            print "length:   %d" % len (words)
            print "# edges:  %d" % num_edges (table)
            print "# trees:  %d" % num_trees (table, 'S', 0, len (words))
    elif command == 'print':
        if words:
            for i, tree in enumerate (recover_trees (table, 'S', 0, len (words))):
                print "parse tree #%s\n%s\n" % (i + 1, treefy (tree))
    else:
        sys.stderr.write ("unknown command: %s\n" % command)
    sys.stdout.write ("> ")
