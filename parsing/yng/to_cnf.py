#!/usr/bin/env python
# to_cnf.py: convert epsilon-free CFG into Chomsky Normal Form
#            (assuming terminals are prefixed by _)
import sys, collections

# read rules and convert them to CNF
i = 1
solo_rules      = {}
unit_rules      = []
unit_rules_prob = {}
l2rs = collections.defaultdict (lambda: collections.defaultdict (float))
for line in sys.stdin:
    rule           = line[:-1].split (" ")
    lhs, rhs, prob = rule[0], rule[1:-1], float (rule[-1])
    if len (rhs) == 1:
        if rhs[0][0] != '_': # handle unit production later
            unit_rules.append (tuple ([lhs, tuple (rhs)]))
            unit_rules_prob[tuple ([lhs, tuple (rhs)])] = [prob, [lhs, rhs[0]]]
    else:
        # replace all terminals with non-terminals
        for j in range (0, len (rhs)):
            if rhs[j][0] == '_':
                lhs_ = 'Z' + rhs[j]
                l2rs[lhs_][tuple (rhs[j:j+1])] = 1.0
                rhs[i] = lhs_
        # generate a rule to replace first two non-terminals to one
        while len (rhs) > 2:
            if tuple (rhs[:2]) not in solo_rules:
                solo_rules[tuple (rhs[:2])] = 'X' + str (i)
                i += 1
            lhs_ = solo_rules[tuple (rhs[:2])]
            l2rs[lhs_][tuple (rhs[:2])] = 1.0
            rhs[:2] = [lhs_]
    l2rs[lhs][tuple (rhs)] = prob

# handle unit productions
while unit_rules:
    rule = unit_rules.pop ()
    (lhs, rhs), (prob, loop) = rule, unit_rules_prob[rule]
    unit_rules_prob.pop (rule)
    l2rs[lhs].pop (rhs)
    for rs, prob_ in l2rs[rhs[0]].items ():
        if len (rs) == 1 and rs[0][0] != '_': # yet unit production
            if rs[0] in loop:
                sys.stderr.write ("loop found: %s\n" % (' -> '.join (loop + rs)))
                sys.exit ()
            else:
                if tuple ([lhs, rs]) not in unit_rules_prob:
                    unit_rules.append (tuple ([lhs, rs]))
                    unit_rules_prob[tuple ([lhs, rs])] = [0, loop]
                unit_rules_prob[tuple ([lhs, rs])][0] += prob * prob_
                unit_rules_prob[tuple ([lhs, rs])][1].append (rs[0])
        l2rs[lhs][rs] += prob * prob_

for lhs in l2rs.keys ():
    for rhs, prob in l2rs[lhs].items ():
        print lhs, ' '.join (rhs), prob
