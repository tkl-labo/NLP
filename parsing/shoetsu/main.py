
# -*- coding: utf-8 -*-

import sys,os
from numpy import get_include
import pyximport; 

models_dir = os.path.dirname(__file__) or os.getcwd()

pyximport.install(setup_args={"include_dirs": [models_dir, get_include()]}, pyimport = True)


from utils import Const,FilePath,Util
from pcfg import PCFG
from earley import Earley

def main(argc, argv):
    sentence =  argv[1] if argc > 1 else "Book the dinner flight"
    #pcfg(sentence)
    earley(sentence)

def earley(sentence):
    earley = Earley()
    #earley.readCFGRules(FilePath.ROOT + "rules_small.txt")
    earley.readCFGRules(FilePath.ROOT + "rules.txt")
    #earley.showRules()
    earley.parse(sentence)
    earley.showStates()
    print("")
    earley.traceRoot()

def pcfg(sentence):
    pcfg = PCFG()
    pcfg.readCFGRules(FilePath.ROOT + "rules.txt")
    #pcfg.showRules()
    pcfg.parse(sentence)
    pcfg.showTrees()
    pass

if __name__ == "__main__":
    argv = sys.argv
    argc = len(argv)
    main(argc, argv)


