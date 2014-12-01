#!/usr/bin/env python
# to_cnf.py: convert epsilon-free CFG rules to CNF
#            (assuming terminals are prefixed by _)
import sys, collections

# read rules and convert them to CNF
i = 1
unit_rules = []
l2rs = collections.defaultdict (set)
for line in sys.stdin:
    rule = line[:-1].split (" ")
    lhs  = rule[0]
    rhs  = rule[1:]
    if ((len (rhs) == 1 and rhs[0][0] == '_') or               # A -> _a
        (len (rhs) == 2 and [r for r in rhs if r[0] == '_'])): # A -> B C
        # output CNF rules as is
        print ' '.join (rule)
    elif len (rhs) == 1:
        # handle unit production later
        if lhs == rhs[0]:
            sys.exit ("self-loop rule: %s" % line)
        unit_rules.append (rule)
    else:
        # replace all terminals with non-terminals
        for j in range (0, len (rhs)):
            if rhs[j][0] == '_':
                lhs_ = 'Z' + rhs[j]
                print "%s %s" % (lhs_, rhs[j])
                rhs[i] = lhs_
        while len (rhs) > 2:
            lhs_ = 'X' + str (i)
            i += 1
            print lhs_, ' '.join (rhs[:2])
            rhs[:2] = [lhs_]
        print lhs, ' '.join (rhs)
    if len (rhs) == 0:
        break
    l2rs[lhs].add (tuple (rhs))

# handle unit productions
while len (unit_rules) > 0:
    rule = unit_rules.pop ()
    lhs = rule[0]
    rhs = rule[1]
    l2rs[lhs].remove (tuple ([rhs]))
    for rs in l2rs[rhs]:
        if len (rs) == 1 and rs[0][0] != '_': # yet unit production
            unit_rules.append ([lhs] + rs)
        else:
            print lhs, ' '.join (rs)
        l2rs[lhs].add (rs)
