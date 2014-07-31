#!/usr/bin/env pypy
# -*- coding:utf-8 -*-
import sys,os
#import math,re,datetime
from ngramCrawler import *
from decimal import *

class Perplexity:
    def __init__(self,testfilename,n):
        # set counts dictionary
        self.n = n
        self.countdic = dict()
        self.sumdic = dict()
        self.vocabulary_size = 0
        self.testfilename = testfilename

        print "start learning"
        #f = open("../data/"+str(n)+"gram.tsv","r")
        f = open("../data/2012-01-01_10","r")
        
        line_num = 0
        for line in f:
            line_num += 1
            if line_num % 10000 == 0:
                print "learning line_num :", line_num

            #items = line.strip().split("\t")
            #ngramtuple = tuple(items[:-1])
            #counts = items[-1]
            items = line.strip().split("\t")
            counts = int(items[0])
            wordlist = items[1].strip().split(" ") # n=1の時は ["I"]のようにlistになる

            #sumdic
            n_1gramtuple = tuple(wordlist[:-1]) # n=1の時は [] 空リスト→  ) 空tupleに
            if n_1gramtuple in self.sumdic:
                self.sumdic[n_1gramtuple]["count"] += counts
                self.sumdic[n_1gramtuple]["types"} += 1
            else:
                self.sumdic[n_1gramtuple] = {"counts":counts,"types":1}

            #countdic
            ngramtuple = tuple(wordlist)
            if ngramtuple in self.countdic:
                self.countdic[ngramtuple] += counts
            else:
                self.countdic[ngramtuple] = counts


    def laplace_prob(self,ngramtuple):
        sumcounts = 0
        counts = 0

        ## calc sumcounts
        n_1gramtuple = tuple(ngramtuple[:-1])
        if n_1gramtuple in self.sumdic:
            sumcoutns = self.sumdic[n_1gramtuple]["counts"] + self.vocabulary_size + 1 #laplace_smoothing + UNK
        else:
            sumcounts = 0 + self.vocabulary_size + 1

        ## calc counts
        if ngramtuple in self.sumdic:
            counts = self.countdic[ngramtuple] + 1 #laplace_smoothing
        else:
            counts = 0 + 1
        
        #return Decimal(counts) / Decimal(sumcounts)
        return float(counts) / sumcounts

    def perplexity(self):
        print "start calcurating perplexity"
        sum_log_prob = 0
        wordcount = 0

        for sentence in dataFetcher(self.testfilename):
            nc = ngramCorpus(sentence)
            for ngram in nc.ngramMaker(self.n):
                prob = self.laplace_prob(ngram)
                sum_log_prob += math.log(prob)
                wordcount += 1

        return math.exp( float((-1)) * sum_log_prob / wordcount) #perplexity

if __name__ == "__main__":
    n = int(sys.argv[1])
    #date = sys.argv[2]

    testfilename = sys.argv[2]
    #px = Perplexity(date,n)
    px = Perplexity(testfilename,n)
    print px.perplexity()
