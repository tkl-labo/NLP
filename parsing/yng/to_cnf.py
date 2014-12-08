#!/usr/bin/env python
# to_cnf.py: convert epsilon-free CFG into Chomsky Normal Form
#            (assuming terminals are prefixed by _)
import sys, collections

# read rules and convert them to CNF
i = 1
unit_rules = []
l2rs = collections.defaultdict (set)
for line in sys.stdin:
    rule           = line[:-1].split (" ")
    lhs, rhs, prob = rule[0], rule[1:-1], float (rule[-1])
    if len (rhs) == 1:
        if rhs[0][0] == '_': # A -> _a
            print ' '.join (rule)
        else:                # handle unit production later
            unit_rules.append (rule)
    else:
        # replace all terminals with non-terminals
        for j in range (0, len (rhs)):
            if rhs[j][0] == '_':
                lhs_ = 'Z' + rhs[j]
                print "%s %s" % (lhs_, rhs[j])
                rhs[i] = lhs_
        # generate a rule to replace first two non-terminals to one
        while len (rhs) > 2:
            lhs_ = 'X' + str (i)
            i += 1
            print lhs_, ' '.join (rhs[:2]), 1.0
            rhs[:2] = [lhs_]
        print lhs, ' '.join (rhs), prob
    l2rs[lhs].add ((tuple (rhs), prob))

# handle unit productions
while unit_rules:
    rule           = unit_rules.pop ()
    lhs, rhs, prob = rule[0], rule[1:-1], float (rule[-1])
    l2rs[lhs].remove ((tuple (rhs), prob))
    for rs, prob_ in l2rs[rhs[0]]:
        if len (rs) == 1 and rs[0][0] != '_': # yet unit production
            unit_rules.append ([lhs, rs, prob * prob_])
        else:
            print lhs, ' '.join (rs), prob * prob_
        l2rs[lhs].add ((rs, prob * prob_))
