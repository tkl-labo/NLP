#!/usr/bin/env python
# -*- coding:utf-8 -*-

import tweet

class ngram:
    
    def __init__(self,n,minimum):
        self.n = n # n gram
        self.minimum = minimum #minimum count for posting on ranking
        self.ngram_sum = 0
        self.ngram = dict()
        self.f = open("../ngram_data/"+str(self.n)+"gram_ranking.tsv","w")

    # divide text into a set of n char
    # ex. "私は大学生です" >(n=4)> ["私は大学","は大学生","大学生で","学生です"]
    def text_to_nchar(self,text): #text needs to be unicode
        ncharlist = []
        if isinstance(text,unicode):
            if len(text) >= self.n:
                for i in range(len(text) - self.n + 1):
                    ncharlist.append(text[i:i+self.n])
                return ncharlist
            else:
                return None
        else:
            print "ERROR:text_to_nchar() needs to be unicode"
            return None

    def append(self,text):
        ncharlist = self.text_to_nchar(text)
        if ncharlist:
            for nchar in ncharlist:
                self.ngram_sum += 1
                if nchar in self.ngram:
                    self.ngram[nchar] += 1
                else:
                    self.ngram[nchar] = 1

    def make_ranking(self,write_flag=False):
        if write_flag == True:
            self.f.write("total"+"\t"+str(self.ngram_sum)+"\n")

        for ncharkey,count in sorted(self.ngram.items(),key= lambda x:x[1], reverse=True):
            if count >= self.minimum:
                print ncharkey.encode("utf8"),count
                if write_flag == True:
                    self.f.write(ncharkey.encode("utf8")+"\t"+str(count)+"\n")
            else:
                break
