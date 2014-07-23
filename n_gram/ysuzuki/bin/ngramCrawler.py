#!/usr/bin/env python
# -*- coding:utf-8 -*-
import sys,os
import math,re,datetime
import subprocess

def test():
    for data in dataFetcher("20120612"):
        print "####"+data+"####\n"
        n = ngramCorpus(data)
        #print n.surface,n.surface[0]
        #print n.postag,n.postag[0]
        #print n.postag_detail,n.postag_detail[0]
        #print "-------------"
        3gram = n.ngramMaker(3)
        print ",".join(3gram)


def dataFetcher(date):
    year = date[0:4]
    month = date[4:6]
    day = date[6:8]
    if len(date) != 8:
        raise ValueError("date must be YYYYMMDD format")

    filepath = "../ngramCorpus/{}/{}/{}.cdr.gz".format(year,month,day)

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
        
        items = sentence.strip().split("\n")[:-2] # EOFは除外
        for item in items:
            surface,description = item.split("\t")
            postag,postag_detail,verb_description,verb_form = description.split(",")
            self.surface.append(surface)
            self.postag.append(postag)
            self.postag_detail.append(postag_detail)
            self.verb_description.append(verb_description)
            self.verb_form.append(verb_form)

    def ngramMaker(self,n):
        ngramlist = []
        if len(self.surface) <= n:
            for i in range(len(self.surface)-(n-1)):
                ngramlist.append(self.surface[i:i+n]
        return ngramlist

if __name__ == "__main__":
    test()
