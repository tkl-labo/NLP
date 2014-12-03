#!/usr/bin/env python
# earley.py: parse given input using Earley algorithm
import sys, collections, itertools

if len (sys.argv) != 2:
    sys.exit ("Usage: %s grammar" % sys.argv[0])

grammar = collections.defaultdict (list) # mapping from lhs to rhs
lexicon = collections.defaultdict (list) # mapping from rhs (terminal) to lhs
pos     = set ()

def add_chart (state, states):
    if state not in states: # linear search to list; slow
        states.append (state)

def parse (words):
    n = len (words)
    chart  = [[] for i in range (n + 1)]
    # initialize
    sys.stderr.write ("Chart[0]\n")
    init_state = ('gamma', ('S',), 0, (0, 0), "Dummy start state")
    add_chart (init_state, chart[0])
    #
    flag = False
    o = 0
    for l in range (n + 1):
        m = 0
        if flag:
            sys.stderr.write ("Chart[%d]\n" % l)
            flag = False
        while m < len (chart[l]):
            state = chart[l][m]
            lhs, rhs, dot, span, op = state
            rhs__ = list (rhs)
            rhs__[dot:dot] = "."
            sys.stderr.write ("%sS%-3d %-30s [%2d,%2d]  %-18s\n" % (" " * 10, o, lhs + " -> " + ' '.join (rhs__), span[0], span[1], op))
            o += 1
            m += 1
            if dot < len (rhs): # incomplete?
                next_cat, (i, j) = rhs[dot], span
                if next_cat not in pos: # predict
                    for rhs in grammar[next_cat]:
                        new_state  = (next_cat, rhs, 0, (j, j), "Predictor")
                        num_states = add_chart (new_state, chart[j])
                else: # scan
                    if j < len (words) and next_cat in lexicon["_" + words[j]]:
                        new_state  = (next_cat, (words[j],), 1, (j, j + 1), "Scanner")
                        num_states = add_chart (new_state, chart[j + 1])
            else: # complete
                j, k = span
                for state in chart[j]:
                    lhs_, rhs_, dot_, (i, j), op = state
                    if dot_ < len (rhs_) and op != "Dummy start state" and rhs_[dot_] == lhs:
                        new_state  = (lhs_, rhs_, dot_ + 1, (i, k), "Completor")
                        num_states = add_chart (new_state, chart[k])
        flag = True
    return chart

def num_states (chart):
    return 0

def num_trees (chart):
    return 0

def recover_trees (chart):
    return ""

def treefy (tree, l = 0):
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
    lhs, rhs = rule[0], rule[1:]
    if len (rhs) == 1 and rhs[0][0] == '_':
        pos.add (lhs)
        lexicon[rhs[0]].append (lhs)
    grammar[lhs].append (tuple (rhs))

# loop
words = []
table = []
sys.stdout.write ("> ")
for line in iter (sys.stdin.readline, ""): # no buffering
    command = line[:-1].split (" ", 1)[0]
    if command == 'parse':
        words = line[:-1].split (" ", 1)[1][1:-1].split (" ")
        chart = parse (words)
        print "success" if [state for state in chart[len (words)] if state[0][0] == 'S' and state[2] == len (state[1]) and state[3] == (0, len (words))] else "fail"
    elif command == 'stat':
        if words:
            print "sentence: %s" % ' '.join (words)
            print "length:   %d" % len (words)
            print "# states: %d" % num_states (chart)
            print "# trees:  %d" % num_trees  (chart)
    elif command == 'print':
        if words:
            i = 1
            for tree in recover_trees (chart):
                print "parse tree #%s\n%s\n" % (i, treefy (tree))
                i += 1
    else:
        sys.stderr.write ("unknown command: %s\n" % command)
    sys.stdout.write ("> ")
