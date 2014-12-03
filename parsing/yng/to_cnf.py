#!/usr/bin/env python
# to_cnf.py: convert epsilon-free CFG into Chomsky Normal Form
#            (assuming terminals are prefixed by _)
import sys, collections

# read rules and convert them to CNF
i = 1
unit_rules = []
l2rs = collections.defaultdict (set)
for line in sys.stdin:
    rule     = line[:-1].split (" ")
    lhs, rhs = rule[0], rule[1:]
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
            print lhs_, ' '.join (rhs[:2])
            rhs[:2] = [lhs_]
        print lhs, ' '.join (rhs)
    l2rs[lhs].add (tuple (rhs))

# handle unit productions
while unit_rules:
    rule     = unit_rules.pop ()
    lhs, rhs = rule
    l2rs[lhs].remove ((rhs, ))
    for rs in l2rs[rhs]:
        if len (rs) == 1 and rs[0][0] != '_': # yet unit production
            unit_rules.append ([lhs] + rs)
        else:
            print lhs, ' '.join (rs)
        l2rs[lhs].add (rs)
