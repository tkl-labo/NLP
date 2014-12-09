#!/usr/bin/env python
# earley.py: parse given input using Earley algorithm
import sys, collections, itertools

if len (sys.argv) != 2:
    sys.exit ("Usage: %s grammar" % sys.argv[0])

grammar = collections.defaultdict (list) # mapping from lhs to rhs
pos     = set ()                         # pre-terminals
npos    = set ()                         # not pre-terminals

def add_chart (state, states, bkptr, r = []):
    if state not in states: # linear search to list; slow
        states.append (state)
        bkptr.append ([-1, []])
    bkptr[states.index (state)][1].extend (r)

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
    bkptr = [[] for i in range (n + 1)] # record how states are generated
    # initialize
    add_chart (('gamma', ('S',), 0, 0), chart[0], bkptr[0])
    # parse
    state_id = 0
    for j in range (n + 1):
        sys.stderr.write ("Chart[%d]:\n" % j)
        m = 0
        while m < len (chart[j]):
            lhs, rhs, dot, k = chart[j][m]
            bkptr[j][m][0] = state_id
            print_state (state_id, chart[j][m], bkptr[j][m], j)
            if dot < len (rhs) and rhs[dot] in npos: # predict
                for rhs_ in grammar[rhs[dot]]:
                    new_state  = (rhs[dot], rhs_, 0, j)
                    add_chart (new_state, chart[j], bkptr[j])
            if dot < len (rhs) and rhs[dot] in pos:  # scan
                if j < n and ("_" + words[j],) in grammar[rhs[dot]]:
                    new_state  = (rhs[dot], (words[j],), 1, j)
                    add_chart (new_state, chart[j + 1], bkptr[j + 1])
            if dot == len (rhs): # complete
                for m_ in range (len (chart[k])): # line search to list; slow
                    lhs_, rhs_, dot_, i = chart[k][m_]
                    state_id_ = bkptr[k][m_][0]
                    if state_id_ and dot_ < len (rhs_) and rhs_[dot_] == lhs:
                        new_state  = (lhs_, rhs_, dot_ + 1, i)
                        add_chart (new_state, chart[j], bkptr[j],
                                   [[[state_id_, [k, m_]], [state_id, [j, m]]]])
            state_id += 1
            m += 1
    # finalize; try to complete dummy initial states
    for m in range (len (chart[n])):
        lhs, rhs, dot, i = chart[n][m]
        state_id = bkptr[n][m][0]
        if lhs == 'S' and dot == len (rhs) and i == 0:
            add_chart (('gamma', ('S', ), 1, 0), chart[n], bkptr[n],
                       [[[0, [0, 0]], [state_id, [n, m]]]])
    return chart, bkptr

def num_states (bkptr): # exclude dummy final state
    return sum (len (c) for c in bkptr) - (1 if bkptr[-1][-1][0] == -1 else 0)

def num_trees (bkptr, i, m):
    if i == m == -1 and bkptr[i][m][0] != -1: # if not parsed
        return 0
    elif not bkptr[i][m][1]:
        return 1 if bkptr[i][m][0] != -1 else 0
    return sum (num_trees (bkptr, k, m_) * num_trees (bkptr, j, m)
                for ((_, (k, m_)), (_, (j, m))) in bkptr[i][m][1])

def recover_trees (chart, bkptr, j, n):
    lhs, rhs, dot, _ = chart[j][n]
    if j == n == -1 and bkptr[j][n][0] != -1: # if not parsed
        pass
    elif not bkptr[j][n][1]: # scanner or predictor (init)
        yield [lhs, rhs[0]] if dot else [lhs]
    else:
        for ((_, (k, m_)), (_, (i, m))) in bkptr[j][n][1]:
            for l, r in itertools.product (recover_trees (chart, bkptr, k, m_),
                                           recover_trees (chart, bkptr, i, m)):
                yield l + [r]

def treefy (tree, l = 0):
    if tree[0][0] == 'X': # reduce rules introduced in converting into CNF
        return "\n".join (treefy (child, l) for child in tree[1:])
    lhs, rhs = tree[0], tree[1:]
    return "%s(%s %s)" % ("  " * l, lhs,
                          rhs[0] if isinstance (rhs[0], str) else
                          '\n'.join ([""] + [treefy (r, l + 1) for r in rhs]))

# read grammar
for line in open (sys.argv[1]):
    rule = line[:-1].split (" ")[:-1] # delete prob
    lhs, rhs = rule[0], rule[1:]
    (pos if len (rhs) == 1 and rhs[0][0] == '_' else npos).add (lhs)
    grammar[lhs].append (tuple (rhs))

# loop
words = []
table = []
sys.stdout.write ("> ")
for line in iter (sys.stdin.readline, ""): # no buffering
    command = line[:-1].split (" ", 1)[0]
    if command == 'parse':
        words = line[:-1].split (" ", 1)[1][1:-1].split (" ")
        chart, bkptr = parse (words)
        print "success" if chart[-1] and chart[-1][-1][0] == 'gamma' else "fail"
    elif command == 'stat':
        if words:
            print "sentence: %s" % ' '.join (words)
            print "length:   %d" % len (words)
            print "# states: %d" % num_states (bkptr)
            print "# trees:  %d" % num_trees  (bkptr, -1, -1)
    elif command == 'print':
        if words:
            for i, tree in enumerate (recover_trees (chart, bkptr, -1, -1)):
                print "parse tree #%s\n%s\n" % (i + 1, treefy (tree[1]))
    else:
        sys.stderr.write ("unknown command: %s\n" % command)
    sys.stdout.write ("> ")
