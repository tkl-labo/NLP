#!/usr/bin/env pypy
# -*- coding:utf-8 -*-
import sys,os
#import math,re,datetime
from ngramCrawler import *
from decimal import *

class Perplexity:
    def __init__(self,date,n):
        # set counts dictionary
        self.n = n
        self.countdic = dict()
        self.sumdic = dict()

        print "start learning"
        f = open("../data/"+str(n)+"gram.tsv","r")
        
        line_num = 0
        for line in f:
            line_num += 1
            if line_num % 10000 == 0:
                print "learning line_num :", line_num

            items = line.strip().split("\t")
            ngramtuple = tuple(items[:-1])
            counts = items[-1]

            #sumdic
            if n > 1:
                pre_ngramtuple = tuple(items[:-2])
                if pre_ngramtuple in self.sumdic:
                    self.sumdic[pre_ngramtuple]["counts"] += counts
                    self.sumdic[pre_ngramtuple]["types"] += 1
                else:
                    self.sumdic[pre_ngramtuple] = {"counts":counts,"types":1}
            else:
                if self.sumdic:
                    self.sumdic["counts"] += counts
                    self.sumdic["types"] += 1
                else:
                    self.sumdic = {"counts":counts,"types":1}

            #countdic
            if ngramtuple in self.countdic:
                self.countdic[ngramtuple] += counts
            else:
                self.countdic[ngramtuple] = counts

            #unknown words
            #print "unk check"
            #for sentence in dataFetcher(date):
            #    nc = ngramCorpus(sentence)
            #    ngramlist = nc.ngramMaker(n)
            #    for ngram in ngramlist:
            #        if ngram in sumdic:
            #            pass
            #        else:
            #            unkset.add(ngram)
            #            unkcounts += 1

    def laplace_prob(self,ngramtuple):
        sumcounts = 0
        counts = 0

        ## calc sumcounts
        if n > 1:
            pre_ngramtuple = tuple(ngramtuple[:-1])
            if pre_ngramtuple in self.sumdic:
                sumcoutns = self.sumdic[pre_ngramtuple]["counts"] + self.sumdic[pre_ngramtuple]["types"] #laplace_smoothing
            else:
                return None # if unknown words exist, skipped
        else: # n = 1
            sumcounts = self.sumdic["counts"]+self.sumdic["types"]

        ## calc counts
        if ngramtuple in self.sumdic:
            counts = self.countdic[ngramtuple] + 1 #laplace_smoothing
        else:
            counts = 1
        
        #return Decimal(counts) / Decimal(sumcounts)
        return float(counts) / sumcounts

    def perplexity(self):
        print "start calcurating perplexity"
        sum_log_prob = 0
        wordcount = 0
        sentence_num = 0
        for sentence in dataFetcher(date):
            sentence_num += 1
            if sentence_num % 10000 == 0:
                print "perplexity sentence_num :", sentence_num

            nc = ngramCorpus(sentence)
            for ngram in nc.ngramMaker(self.n):
                prob = self.laplace_prob(ngram)
                if prob:
                    sum_log_prob += math.log(prob)
                    wordcount += 1
                else:
                    pass

        return math.exp( float((-1)) * sum_log_prob / wordcount) #perplexity

if __name__ == "__main__":
    n = int(sys.argv[1])
    date = sys.argv[2]

    px = Perplexity(date,n)
    print px.perplexity()
