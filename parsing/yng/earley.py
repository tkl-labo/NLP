#!/usr/bin/env python
# earley.py: parse given input using Earley algorithm
import sys, collections, itertools

if len (sys.argv) != 2:
    sys.exit ("Usage: %s grammar" % sys.argv[0])

grammar = collections.defaultdict (list) # mapping from lhs to rhs
pos     = set ()                         # pre-terminals

def add_chart (state, states, rel, r = []):
    if state not in states: # linear search to list; slow
        states.append (state)
        rel.append ([-1, []])
    if r:
        rel[states.index (state)][1].append (r)

def print_state (state_id, state, rs):
    lhs, rhs, dot, span = (list (x) if isinstance (x, tuple) else x for x in state)
    rhs[dot:dot] = '.'
    sys.stderr.write ("%sS%-3d " % (" " * 4, state_id))
    sys.stderr.write ("%-26s  "  % (' '.join ([lhs, '->'] + rhs)))
    sys.stderr.write ("[%s]  "   % (','.join ("%2d" % i for i in span)))
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
    sys.stderr.write ("Chart[0]\n")
    init_state = ('gamma', ('S',), 0, (0, 0))
    add_chart (init_state, chart[0], rel[0])
    #
    flag     = False
    state_id = 0
    for l in range (n + 1):
        m = 0
        if flag:
            sys.stderr.write ("Chart[%d]:\n" % l)
            flag = False
        while m < len (chart[l]):
            lhs, rhs, dot, span = chart[l][m]
            rel[l][m][0] = state_id
            print_state (state_id, chart[l][m], rel[l][m])
            if dot < len (rhs): # incomplete state?
                next_cat, (i, j) = rhs[dot], span
                if next_cat not in pos: # predict
                    for rhs in grammar[next_cat]:
                        new_state  = (next_cat, rhs, 0, (j, j))
                        add_chart (new_state, chart[j], rel[j])
                else: # scan
                    if j < len (words) and ("_" + words[j],) in grammar[next_cat]:
                        new_state  = (next_cat, (words[j],), 1, (j, j + 1))
                        add_chart (new_state, chart[j + 1], rel[j + 1])
            else: # complete
                j, k = span
                for m_ in range (len (chart[j])):
                    lhs_, rhs_, dot_, (i, j) = chart[j][m_]
                    state_id_ = rel[j][m_][0]
                    if state_id_ and dot_ < len (rhs_) and rhs_[dot_] == lhs:
                        new_state  = (lhs_, rhs_, dot_ + 1, (i, k))
                        add_chart (new_state, chart[k], rel[k], [[state_id_, [j, m_]], [state_id, [l, m]]])
            state_id += 1
            m += 1
        flag = True
    return chart, rel

def end_state (chart, rel, n):
     return [[[0, [0, 0]], [rel[n][m][0], [n, m]]]
             for m, (lhs, rhs, dot, span) in enumerate (chart[n])
             if lhs == 'S' and dot == len (rhs) and span == (0, n)]

def num_states (chart):
    return sum (len (c) for c in chart)

def num_trees (rel, rs):
    if not rs[1]:
        return 1 if rs[0] != -1 else 0
    return sum (num_trees (rel, rel[i][j]) *
                num_trees (rel, rel[i_][j_])
                for ((_, (i, j)), (_, (i_, j_))) in rs[1])

def recover_trees (chart, rel, rs, beg, end):
    if not rs[1]:
        if chart[beg][end][2]:
            yield [chart[beg][end][0], chart[beg][end][1][0]]
        else:
            yield []
    elif rs[0] == -1:
        for (_, (_, (i, j))) in rs[1]:
            for tree in recover_trees (chart, rel, rel[i][j], i, j):
                yield tree
    else:
        for ((_, (i, j)), (_, (i_, j_))) in rs[1]:
            for l, r in itertools.product (recover_trees (chart, rel, rel[i][j], i, j),
                                           recover_trees (chart, rel, rel[i_][j_], i_, j_)):
                yield [chart[beg][end][0]] +  l + [r] if chart[beg][end][2] == len (chart[beg][end][1]) else l + [r]

def treefy (tree, l = 0):
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
    lhs, rhs = rule[0], rule[1:]
    if len (rhs) == 1 and rhs[0][0] == '_':
        pos.add (lhs)
    grammar[lhs].append (tuple (rhs))

# loop
words = []
table = []
sys.stdout.write ("> ")
for line in iter (sys.stdin.readline, ""): # no buffering
    command = line[:-1].split (" ", 1)[0]
    if command == 'parse':
        words = line[:-1].split (" ", 1)[1][1:-1].split (" ")
        chart, rel = parse (words)
        print "success" if end_state (chart, rel, len (words)) else "fail"
    elif command == 'stat':
        if words:
            print "sentence: %s" % ' '.join (words)
            print "length:   %d" % len (words)
            print "# states: %d" % num_states (chart)
            print "# trees:  %d" % num_trees  (rel, [-1, end_state (chart, rel, len (words))])
    elif command == 'print':
        if words:
            i = 1
            for tree in recover_trees (chart, rel, [-1, end_state (chart, rel, len (words))], -1, -1):
                print "parse tree #%s\n%s\n" % (i, treefy (tree))
                i += 1
    else:
        sys.stderr.write ("unknown command: %s\n" % command)
    sys.stdout.write ("> ")
