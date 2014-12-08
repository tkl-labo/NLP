#!/usr/bin/env python
# to_noprob.py: convert probabilistic CFG into CFG
import sys

for line in sys.stdin:
    print ' '.join (line.split(' ')[:-1])
    
