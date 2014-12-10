#!/usr/bin/env python
# earley.py: parse given input using Earley algorithm
import sys, collections, itertools

if len (sys.argv) != 2:
    sys.exit ("Usage: %s grammar" % sys.argv[0])

grammar = collections.defaultdict (lambda: {}) # mapping from lhs to rhs
pos     = set ()                               # pre-terminals
npos    = set ()                               # not pre-terminals

def add_chart (state, states, check, nextl, bkptr, prob = 1.0, r = []):
    if state not in check:
        check[state] = len (states)
        if state[2] < len (state[1]):
            nextl[state[1][state[2]]].append (len (states))
        states.append (state)
        bkptr.append ([-1, [], 0.0])
    if bkptr[check[state]][2] < prob:
        bkptr[check[state]][1] = r
        bkptr[check[state]][2] = prob

def parse (words):
    n     = len (words)
    chart = [[] for i in range (n + 1)]
    nextl = [collections.defaultdict (list) for i in range (n + 1)]
    check = [{} for i in range (n + 1)]
    bkptr = [[] for i in range (n + 1)] # record how states are generated
    # initialize
    add_chart (('gamma', ('S',), 0, 0), chart[0], check[0], nextl[0], bkptr[0])
    # parse
    state_id = 0
    for j in range (n + 1):
        m = 0
        completed = []
        while m < len (chart[j]): # unit production? S -> V, S -> VP -> V
            lhs, rhs, dot, k = chart[j][m]
            bkptr[j][m][0] = state_id
            if dot < len (rhs) and rhs[dot] in npos: # predict
                for rhs_, prob in grammar[rhs[dot]].items ():
                    new_state  = (rhs[dot], rhs_, 0, j)
                    add_chart (new_state, chart[j], check[j], nextl[j], bkptr[j], prob)
            if dot < len (rhs) and rhs[dot] in pos:  # scan
                if j < n and ("_" + words[j],) in grammar[rhs[dot]]:
                    new_state  = (rhs[dot], (words[j],), 1, j)
                    add_chart (new_state, chart[j + 1], check[j + 1], nextl[j + 1], bkptr[j + 1], grammar[rhs[dot]][("_" + words[j],)])
            if dot == len (rhs): # complete
                completed.append (m)
                for m_ in nextl[k][lhs]:
                    lhs_, rhs_, dot_, i = chart[k][m_]
                    state_id_ = bkptr[k][m_][0]
                    if state_id_:
                        new_state  = (lhs_, rhs_, dot_ + 1, i)
                        add_chart (new_state, chart[j], check[j], nextl[j], bkptr[j],
                                   bkptr[k][m_][2] * bkptr[j][m][2],
                                   [[state_id_, [k, m_]], [state_id, [j, m]]])
            state_id += 1
            m += 1
        while completed:
            m = completed.pop ()
            lhs, rhs, dot, k = chart[j][m]
            for m_ in nextl[k][lhs]:
                lhs_, rhs_, dot_, i = chart[k][m_]
                state_id_ = bkptr[k][m_][0]
                if state_id_:
                    new_state  = (lhs_, rhs_, dot_ + 1, i)
                    prob = bkptr[k][m_][2] * bkptr[j][m][2]
                    bk   = bkptr[j][check[j][new_state]]
                    if bk[2] < prob:
                        bk[1] = [[state_id_, [k, m_]], [state_id, [j, m]]]
                        bk[2] = prob
                        completed.append (m)
    # finalize; try to complete dummy initial states
    for m in range (len (chart[n])):
        lhs, rhs, dot, i = chart[n][m]
        state_id = bkptr[n][m][0]
        if lhs == 'S' and dot == len (rhs) and i == 0:
            add_chart (('gamma', ('S', ), 1, 0), chart[n], check[n], nextl[n], bkptr[n],
                       bkptr[0][0][2] * bkptr[n][m][2],
                       [[0, [0, 0]], [state_id, [n, m]]])
    return chart, bkptr

def build_tree (chart, bkptr, j, n):
    lhs, rhs, dot, _ = chart[j][n]
    if not bkptr[j][n][1]: # scanner or predictor (init)
        return [lhs, rhs[0]] if dot else [lhs]
    ((_, (k, m_)), (_, (i, m))) = bkptr[j][n][1]
    return build_tree (chart, bkptr, k, m_) + [build_tree (chart, bkptr, i, m)]

def treefy (tree, l = 0):
    if tree[0][0] == 'X': # reduce rules introduced in converting into CNF
        return "\n".join (treefy (child, l) for child in tree[1:])
    lhs, rhs = tree[0], tree[1:]
    return "%s(%s %s)" % ("  " * l, lhs,
                          rhs[0] if isinstance (rhs[0], str) else
                          '\n'.join ([""] + [treefy (r, l + 1) for r in rhs]))

# read grammar
for line in open (sys.argv[1]):
    rule = line[:-1].split (" ")
    lhs, rhs, prob = rule[0], rule[1:-1], float (rule[-1])
    (pos if len (rhs) == 1 and rhs[0][0] == '_' else npos).add (lhs)
    grammar[lhs][tuple (rhs)] = prob

# loop
words = sys.stdin.read ()[:-1].split (' ')
chart, bkptr = parse (words)
if words and bkptr[-1][-1][0] == -1:
    print "prob: %g" % bkptr[-1][-1][2]
    print treefy (build_tree (chart, bkptr, -1, -1)[1])
