#!/usr/bin/env python
# earley.py: parse given input using Earley algorithm
import sys, collections, itertools

if len (sys.argv) != 2:
    sys.exit ("Usage: %s grammar" % sys.argv[0])

grammar = collections.defaultdict (lambda: {}) # mapping from lhs to rhs
pos     = set ()                               # pre-terminals
npos    = set ()                               # not pre-terminals

def add_chart (state, states, rel, prob = 0.0, r = []):
    if state not in states: # linear search to list; slow
        states.append (state)
        rel.append ([-1, [], 0.0])
    if rel[states.index (state)][2] < prob:
        rel[states.index (state)][1] = r
        rel[states.index (state)][2] = prob

# for debugging
def print_state (state_id, state, rs, j):
    lhs, rhs, dot, i = (list (x) if isinstance (x, tuple) else x for x in state)
    rhs[dot:dot] = '.'
    sys.stderr.write ("%sS%-3d "    % (" " * 4, state_id))
    sys.stderr.write ("%-26s  "     % (' '.join ([lhs, '->'] + rhs)))
    sys.stderr.write ("[%2d,%2d]  " % (i, j))
    if   rs[1]: sys.stderr.write ("Completer")
    elif dot:   sys.stderr.write ("Scanner")
    elif rs[0]: sys.stderr.write ("Predictor")
    else:       sys.stderr.write ("Init")
    sys.stderr.write ("\n")

def parse (words):
    n     = len (words)
    chart = [[] for i in range (n + 1)]
    rel   = [[] for i in range (n + 1)] # record how states are generated
    # initialize
    add_chart (('gamma', ('S',), 0, 0), chart[0], rel[0])
    # parse
    state_id = 0
    for j in range (n + 1):
        sys.stderr.write ("Chart[%d]:\n" % j)
        m = 0
        while m < len (chart[j]):
            lhs, rhs, dot, k = chart[j][m]
            rel[j][m][0] = state_id
            print_state (state_id, chart[j][m], rel[j][m], j)
            if dot < len (rhs) and rhs[dot] in npos: # predict
                for rhs_, prob in grammar[rhs[dot]].items ():
                    new_state  = (rhs[dot], rhs_, 0, j)
                    add_chart (new_state, chart[j], rel[j], prob)
            if dot < len (rhs) and rhs[dot] in pos:  # scan
                if j < n and ("_" + words[j],) in grammar[rhs[dot]]:
                    new_state  = (rhs[dot], (words[j],), 1, j)
                    add_chart (new_state, chart[j + 1], rel[j + 1], grammar[rhs[dot]][("_" + words[j],)])
            if dot == len (rhs): # complete
                for m_ in range (len (chart[k])): # line search to list; slow
                    lhs_, rhs_, dot_, i = chart[k][m_]
                    state_id_ = rel[k][m_][0]
                    if state_id_ and dot_ < len (rhs_) and rhs_[dot_] == lhs:
                        new_state  = (lhs_, rhs_, dot_ + 1, i)
                        add_chart (new_state, chart[j], rel[j],
                                   rel[k][m_][2] * rel[j][m][2],
                                   [[state_id_, [k, m_]], [state_id, [j, m]]])
            state_id += 1
            m += 1
    # finalize; try to complete dummy initial states
    for m in range (len (chart[n])):
        lhs, rhs, dot, i = chart[n][m]
        state_id = rel[n][m][0]
        if lhs == 'S' and dot == len (rhs) and i == 0:
            add_chart (('gamma', ('S', ), 1, 0), chart[n], rel[n],
                       1.0 * rel[n][m][2],
                       [[0, [0, 0]], [state_id, [n, m]]])
    return chart, rel

def build_tree (chart, rel, j, n):
    lhs, rhs, dot, _ = chart[j][n]
    if not rel[j][n][1]: # scanner or predictor (init)
        return [lhs, rhs[0]] if dot else [lhs]
    ((_, (k, m_)), (_, (i, m))) = rel[j][n][1]
    return build_tree (chart, rel, k, m_) + [build_tree (chart, rel, i, m)]

def treefy (tree, l = 0):
    if tree[0][0] == 'X': # reduce rules introduced in converting into CNF
        return "\n".join (treefy (child, l) for child in tree[1:])
    lhs, rhs = tree[0], tree[1:]
    ret = "%s(%s" % ("  " * l, lhs)
    if isinstance (rhs[0], str): # terminal
        ret += " %s)" % rhs[0]
    else:
        ret += "\n%s)" % ('\n'.join (treefy (r, l + 1) for r in rhs))
    return ret

# read grammar
for line in open (sys.argv[1]):
    rule = line[:-1].split (" ")
    lhs, rhs, prob = rule[0], rule[1:-1], float (rule[-1])
    (pos if len (rhs) == 1 and rhs[0][0] == '_' else npos).add (lhs)
    grammar[lhs][tuple (rhs)] = prob

# loop
words = sys.stdin.read ()[:-1].split (' ')
chart, rel = parse (words)
if words and rel[-1][-1][0] == -1:
    print "prob: %g" % rel[-1][-1][2]
    print treefy (build_tree (chart, rel, -1, -1)[1])
