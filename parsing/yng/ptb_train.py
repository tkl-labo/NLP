#!/usr/bin/env python
# ptb_train.py -- train PCFG from Penn Treebank
#   usage: python ptb_train.py < train.mrg > rule_ptb.txt 2> train.sent
#     stdout: grammar
#     stderr: sentence
import sys, re, collections

def count_rule_and_replace (m, grammar, words, flag):
    rule = re.sub (r'\s+', ' ', m.group (1).lstrip ().rstrip ())
    lhs, rhs = rule.split (" ", 1)
    if flag:
        word = rhs if re.match (r'NNP', lhs) else rhs.lower ()
        words.append (word)
        rhs = "_" + word
    grammar[lhs][rhs] += 1
    return lhs

tree       = ""
words      = []
copen      = 0
cclose     = 0
l2rs       = collections.defaultdict (lambda: collections.defaultdict (int))
for line in sys.stdin:
    tree += line[:-1]
    copen  += line.count ("(")
    cclose += line.count (")")
    if tree and copen == cclose:
        words = []
        # remove -NONE-
        tree = re.sub (r'\(-NONE- [^()]+\)', '', tree)
        while re.search (r'\([^() ]+ *\)', tree):
            tree = re.sub (r'\([^() ]+ *\)', '', tree)
        # remove functional tags
        tree = re.sub (r'\(([^() -=]+)[^() ]*', r'(\1', tree)
        # handle terminals
        tree = re.sub (r'\(([^()]+)\)',
                       lambda m: count_rule_and_replace (m, l2rs, words, True), tree)
        # handle constituents
        while re.search (r'\([^()]+ [^()]+\)', tree):
            tree = re.sub (r'\(([^()]+ [^()]+)\)',
                           lambda m: count_rule_and_replace (m, l2rs, words, False), tree)
        sys.stderr.write (' '.join (words) + "\n")
        tree  = ""
        words = []
        copen = cclose = 0

for lhs in l2rs.keys ():
       total = sum (l2rs[lhs].values ())
       for rhs, count in l2rs[lhs].items ():
           # if lhs == rhs: continue
           print "%s %s %f" % (lhs, rhs, float (count) / total)
