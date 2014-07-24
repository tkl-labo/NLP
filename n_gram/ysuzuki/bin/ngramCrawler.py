#!/usr/bin/env pypy
# -*- coding:utf-8 -*-
import sys,os
import math,re,datetime
import subprocess,time

def test():
    for data in dataFetcher("20120612"):
        #print "####"+data+"####\n"
        n = ngramCorpus(data)
        print n.surface,n.surface[0]
        #print n.postag,n.postag[0]
        #print n.postag_detail,n.postag_detail[0]
        #print "-------------"
        gram = n.ngramMaker(3)
        #print gram,type(gram)
        if gram != []:
            for g in gram:
                print ",".join(g)
        else:
            print []


def dataFetcher(date):
    year = date[0:4]
    month = date[4:6]
    day = date[6:8]
    if len(date) != 8:
        raise ValueError("date must be YYYYMMDD format")

    filepath = "../ngramCorpus/{}/{}/{}.cdr.gz".format(year,month,day)

    #only in test
    #filepath = "./test.gz"

    # file open
    #高速化のため，gzipではなくsubprocessでzcatを利用
    #f = gzip.open(filepath,"r")
    print "OPEN FILE: {}".format(filepath)
    if sys.version.startswith("3"):
        import io
        io_method = io.BytesIO
    else:
        import cStringIO
        io_method = cStringIO.StringIO
    p = subprocess.Popen(["zcat",filepath],stdout = subprocess.PIPE)
    f = io_method(p.communicate()[0])

    while True:
        sentence = ""
        line = f.readline()
        if line == "": # the end of a file
            raise StopIteration

        while line.strip() != "EOS":
            sentence += line
            line = f.readline()

        sentence += line
        yield sentence

class ngramCorpus:
    def __init__(self,sentence):
        self.surface = []
        self.postag = []
        self.postag_detail = []
        self.verb_description = []
        self.verb_form = []
        
        items = sentence.strip().split("\n")[:-1] # EOFは除外
        for item in items:
            surface,description = item.split("\t")
            postag,postag_detail,verb_description,verb_form = description.split(",")
            self.surface.append(surface)
            self.postag.append(postag)
            self.postag_detail.append(postag_detail)
            self.verb_description.append(verb_description)
            self.verb_form.append(verb_form)

    def ngramMaker(self,n,with_pseudo_flag=True):
        ngramlist = []
        if len(self.surface) < n:
            return []

        if with_pseudo_flag: #文頭文末を<s>,</s>と擬似的な単語として扱う
            surface = ["<s>"] * (n - 1) + self.surface + ["</s>"] * (n - 1)
            for i in range(len(surface)-(n-1)):
                ngramlist.append(tuple(surface[i:i+n]))
            return ngramlist
        else: # <s>. </s> をつけない
            for i in range(len(self.surface)-(n-1)):
                ngramlist.append(tuple(self.surface[i:i+n]))
            return ngramlist

# date とn を指定，一気に1~n gramまでcountし，tsv fileに書き込む
def counter(date,n):
    # initialize
    start_time = time.clock()

    fpathlist = []
    for i in range(1,n+1):
        path = open("../works/"+str(i)+"gram/"+date,"w")
        fpathlist.append(path)

    ngramCountDics = []
    for i in range(1,n+1):
        ngramCountDics.append(dict())

    # count
    sentence_number = 0
    for sentence in dataFetcher(date):
        sentence_number += 1
        if sentence_number % 100000 == 0:
            print "{} {} {:.1f}min".format(date,sentence_number,(time.clock()-start_time)/60)

        nc = ngramCorpus(sentence)
        for i in range(1,n+1):
            ngramlist = nc.ngramMaker(i+1) # ngramをcountしたいとき，n + 1 の連続した語が必要
            for ngram in ngramlist:
                if ngram in ngramCountDics[i-1]: # 0 gramはないので，数字の都合上 i-1
                    ngramCountDics[i-1][ngram] += 1
                else:
                    ngramCountDics[i-1][ngram] = 1


    # write the result to tsv file
    print "WRITE THE RESULT"
    for i in range(1,n+1):
        print i,len(ngramCountDics[i-1])
        for ngram in ngramCountDics[i-1]:
            fpathlist[i-1].write("\t".join(ngram) + "\t" + str(ngramCountDics[i-1][ngram]) + "\n")

    # all done log (for makefile)
    flog = open("../works/log/counter/"+date,"w")
    flog.close()

if __name__ == "__main__":
    #test()
    cmd = sys.argv[1]
    date = sys.argv[2]

    if cmd == "counter":
        counter(date,3)
    elif cmd == "merger":
        pass
