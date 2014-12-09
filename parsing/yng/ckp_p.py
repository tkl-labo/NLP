#!/usr/bin/env python
# cky.py: parse given input using CKY algorithm
import sys, collections, itertools

if len (sys.argv) != 2:
    sys.exit ("Usage: %s grammar" % sys.argv[0])

grammar = collections.defaultdict (list) # mapping from rhs to [lhs, prob]

def parse (words):
    n = len (words)
    table = [[collections.defaultdict (float) for j in range (n + 1)]
             for i in range (n + 1)]
    back  = [[collections.defaultdict (list)  for j in range (n + 1)]
             for i in range (n + 1)]
    # parse
    for j in range (1, n + 1):
        for lhs, prob in grammar[("_" + words[j - 1], )]:
            table[j - 1][j][lhs] = prob
            back[j - 1][j][lhs]  = (j - 1, words[j - 1])
        for i in reversed (range (j)):
            for k in range (i + 1, j):
                for rhs in itertools.product (table[i][k], table[k][j]):
                    for lhs, prob in grammar[rhs]:
                        prob_ = table[i][k][rhs[0]] * table[k][j][rhs[1]] * prob
                        if table[i][j][lhs] < prob_:
                            table[i][j][lhs] = prob_
                            back[i][j][lhs]  = (k, rhs)
    return table, back

def build_tree (back, label, beg, end):
    if end - beg == 1: # terminal
        return [label, back[beg][end][label][1]]
    else:
        k, rhs = back[beg][end][label]
        return [label,
                build_tree (back, rhs[0], beg, k),
                build_tree (back, rhs[1], k, end)]

def treefy (tree, l = 0):
    if tree[0][0] == 'X': # reduce rules introduced in converting into CNF
        return "\n".join (treefy (child, l) for child in tree[1:])
    lhs, rhs = tree[0], tree[1:]
    ret = "%s(%s" % ("  " * l, lhs)
    if len (rhs) == 1: # terminal
        ret += " %s)" % rhs[0]
    else:
        ret += "\n%s\n%s)" % (treefy (rhs[0], l + 1), treefy (rhs[1], l + 1))
    return ret

# read grammar
for line in open (sys.argv[1]):
    rule = line[:-1].split (" ")
    lhs, rhs, prob = rule[0], rule[1:-1], float (rule[-1])
    grammar[tuple (rhs)].append ((lhs, prob))

# loop
words = sys.stdin.read ()[:-1].split (' ')
table, back = parse (words)
if words and back[0][len (words)]['S']:
    print "prob: %g" % table[0][len (words)]['S']
    print treefy (build_tree (back, 'S', 0, len (words)))
