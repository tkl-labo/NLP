#!/usr/bin/env python
# to_cnf.py: convert epsilon-free CFG into Chomsky Normal Form
#            (assuming terminals are prefixed by _)
import sys, collections

if len (sys.argv) >= 3:
    sys.exit ("Usage: %s [noconv_unit]" % sys.argv[0])

grammar     = []
new_grammar = collections.defaultdict (float)
l2rs        = collections.defaultdict (list)

# read rules
for line in sys.stdin:
    rule = line[:-1].split (" ")
    lhs, rhs, prob = rule[0], rule[1:-1], float (rule[-1])
    l2rs[lhs].append (len (grammar))
    grammar.append ([lhs, rhs, prob])

# convert terminals within non-lexical rules to dummy non-terminals
for lhs, rhs, prob in grammar:
    if len (rhs) >= 2:
        for j in range (0, len (rhs)):
            if rhs[j][0] == '_':
                lhs_ = 'Z' + rhs[j]
                new_grammar[(lhs_, tuple (rhs[j:j + 1]))] = 1.0
                rhs[j] = lhs_

# convert unit productions; assuming no loop
replaced = set ()
if len (sys.argv) == 1:
    j = 0
    while j < len (grammar):
        lhs, rhs, prob = grammar[j]
        if len (rhs) == 1 and rhs[0][0] != '_':
            for k in l2rs[rhs[0]]:
                if k not in replaced:
                    _, rhs_, prob_ = grammar[k]
                    l2rs[lhs].append (len (grammar))
                    grammar.append ([lhs, rhs_, prob * prob_])
            replaced.add (j)
        j += 1

# binarize all rules and add to new grammar
solo_rules = {}
i = 1
for j in range (len (grammar)):
    if j in replaced:
        continue
    lhs, rhs, prob = grammar[j]
    while len (rhs) >= 3:
        if tuple (rhs[:2]) not in solo_rules:
            lhs_ = 'X' + str (i)
            solo_rules[tuple (rhs[:2])] = lhs_
            new_grammar[(lhs_, tuple (rhs[:2]))] = 1.0
            i += 1
        else:
            lhs_ = solo_rules[tuple (rhs[:2])]
        rhs[:2] = [lhs_]
    new_grammar[(lhs, tuple (rhs))] += prob

# output new_grammar
for (lhs, rhs), prob in new_grammar.items ():
    print lhs, ' '.join (rhs), prob
