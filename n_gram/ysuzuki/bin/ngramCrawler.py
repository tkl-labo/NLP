#!/usr/bin/env pypy
# -*- coding:utf-8 -*-
import sys,os
import math,re,datetime
import subprocess,time
import glob

def test():
    for data in dataFetcher("20120612"):
        #print "####"+data+"####\n"
        n = ngramCorpus(data)
        print n.surface,n.surface[0].encode("utf8")
        #print n.postag,n.postag[0]
        #print n.postag_detail,n.postag_detail[0]
        #print "-------------"
        gram = n.ngramMaker(3)
        #print gram,type(gram)
        if gram != []:
            for g in gram:
                print u",".join(g).encode("utf8")
        else:
            print []


def dataFetcher(date):
    year = date[0:4]
    month = date[4:6]
    day = date[6:8]
    if len(date) != 8:
        raise ValueError("date must be YYYYMMDD format")

    #filepath = "../ngramCorpus/{}/{}/{}.cdr.gz".format(year,month,day)

    #only in test
    #filepath = "./test.gz"
    filepath = "/home/ynaga/ngrams/data/2012-01-01.cdr.1000000.gz"

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
        try:
            sentence = ""
            line = f.readline().decode("utf8")

            if line == u"": # the end of a file
                raise StopIteration

            while line.strip() != u"EOS":
                sentence += line
                line = f.readline().decode("utf8")

            sentence += line
            yield sentence
        except UnicodeDecodeError:
            #while line.strip() != u"EOS":
            #    line = f.readline()
            #continue
            print "UnicodeDecodeError"
            print line.encode("utf8")

class ngramCorpus:
    def __init__(self,sentence):
        self.surface = []
        self.postag = []
        self.postag_detail = []
        self.verb_description = []
        self.verb_form = []
        
        items = sentence.strip().split(u"\n")[:-1] # EOFは除外
            
        for item in items:
            surface,description = item.split(u"\t")
            postag,postag_detail,verb_description,verb_form = description.split(u",")
            self.surface.append(surface)
            self.postag.append(postag)
            self.postag_detail.append(postag_detail)
            self.verb_description.append(verb_description)
            self.verb_form.append(verb_form)

    def ngramMaker(self,n,with_pseudo_flag=True):
        ngramlist = []
        #if len(self.surface) + (n-1) + 1 < n:
        #    return []

        if with_pseudo_flag: #文頭文末を<s>,</s>と擬似的な単語として扱う
            surface = [u"<s>"] * (n - 1) + self.surface + [u"</s>"] * 1
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
            ngramlist = nc.ngramMaker(i) # ngramlist
            for ngram in ngramlist:
                if ngram in ngramCountDics[i-1]: # 0 gramはないので，数字の都合上 i-1
                    ngramCountDics[i-1][ngram] += 1 
                else:
                    ngramCountDics[i-1][ngram] = 1


    # write the result to tsv file
    print "WRITE THE RESULT"
    for i in range(1,n+1):
        print i,len(ngramCountDics[i-1])
        for ngram in sorted(ngramCountDics[i-1]):
            #fpathlist[i-1].write(u"\t".join(ngram).encode("utf8") + "\t" + str(ngramCountDics[i-1][ngram]) + "\n")
            fpathlist[i-1].write(str(ngramCountDics[i-1][ngram]) + "\t" + " ".join(ngram).encode("utf8") + "\n")

    # all done log (for makefile)
    flog = open("../works/log/counter/"+date+".done","w")
    flog.close()

def merger(n):
    print "{} gram merger start".format(n)
    ngramdic = dict()
    
    filepath = "../works/"+str(n)+"gram/[0-9]*"
    files = glob.glob(filepath)

    for fname in sorted(files):
        f = open(fname,"r")
        print "{}gram merger : {} open".format(n,fname)
        for line in f:
            try:
                items = line.decode("utf8").strip().split(u"\t")
            except UnicodeDecodeError:
                print "unicodedecodeerror"
                continue

            ngram = tuple(items[:-1])
            count = int(items[-1])

            # 3gramの集計はメモリがきついので，各日1,2回しか現れない単語頻度は切り捨て
            if n == 3:
                if count >= 3:
                    pass
                else:
                    continue

            if n == 2:
                if count >= 2:
                    pass
                else:
                    continue

            if ngram in ngramdic:
                ngramdic[ngram] += count
            else:
                ngramdic[ngram] = count
            
    #writing
    f_out = open("../data/"+str(n)+"gram.tsv","w")
    print "{}gram writing start".format(n)
    print len(ngramdic)
    for ngram,counts in sorted(ngramdic.items(),key = lambda x : x[1],reverse=True):
        ngram_utf8 = []
        for word in ngram:
            ngram_utf8.append(word.encode("utf8"))
        output = "\t".join(ngram_utf8) + "\t" + str(counts) + "\n"
        f_out.write(output)

if __name__ == "__main__":

    if len(sys.argv) != 3:
        print "Usage : counter date\n",
        print "      : merger n"
        quit()

    #test()
    cmd = sys.argv[1]

    if cmd == "counter":
        date = sys.argv[2]
        counter(date,3)
    elif cmd == "merger":
        n = int(sys.argv[2])
        merger(n)
