import sys
from grammer_prob import *
import itertools
from collections import namedtuple
import math
Cell=namedtuple('Cell',['Tag','x','y','prob'])
BackPointer=namedtuple('BackPointer',['Left','Right','prob'])
class CKYparser:
    def __init__(self,words,grammer):
        self.words = words
        self.grammer = grammer
        self.table = [[{}]*(len(words)+1) for i in xrange((len(words)))]
    def mktable(self):
        for i in xrange(len(self.words)):
            for j in xrange(i+1):
                if j == 0:
                    for x in self.grammer.taglist('_'+self.words[i]):
                        self.table[i][i+1] = {x:[Cell(Tag=self.words[i],x=i,y=i+1,prob=math.log(self.grammer.calc_prob(x,('_'+self.words[i],))))] for x in self.grammer.taglist('_'+self.words[i]) }
                else:
                    taglist = {}
                    for k in xrange(j):
                        if self.table[i-j][i-k] =='' or self.table[i-k][i+1] =='':
                            continue
                        for tagA in self.table[i-j][i-k]:
                            for tagB in self.table[i-k][i+1]:
                                for x in self.grammer.search_gram(tagA,tagB):
                                    if taglist.has_key(x):
                                        L=min(self.table[i-j][i-k][tagA],key=(lambda x: x.prob))
                                        R=min(self.table[i-k][i+1][tagB],key=(lambda x: x.prob))
                                        taglist[x].append(BackPointer(Cell(tagA,i-j,i-k,L.prob),Cell(tagB,i-k,i+1,R.prob),prob=math.log(self.grammer.calc_prob(x,(tagA,tagB)))+L.prob+R.prob))
                                    else:
                                        L=min(self.table[i-j][i-k][tagA],key=(lambda x: x.prob))
                                        R=min(self.table[i-k][i+1][tagB],key=(lambda x: x.prob))
                                        taglist.update( { x:[BackPointer(Cell(tagA,i-j,i-k,L.prob),Cell(tagB,i-k,i+1,R.prob),L.prob+R.prob)]})
                    if(len(taglist)):
                        tmp = {}
                        for x in taglist.items():
                            tmp.update({x[0]:[min(x[1],key=(lambda y: y.prob))]})
                        self.table[i-j][i+1] = tmp
                    else:
                        pass
def mktree(node,mark):
    for x in node[mark]:
        if len(x) ==3:
            for l, r in itertools.product(mktree(Parser.table[x[0][1]][x[0][2]],x[0][0]), mktree(Parser.table[x[1][1]][x[1][2]],x[1][0])):
                yield [mark,l,r]
        else:
            yield [mark,x[0]]
        if mark == 'S':
            pass

def c_edge(table):
    count = 0
    for row in table:
        for cell in row:
            for i,j in cell.items():
                for k in j:
                    if len(k)==2:
                        count += 1
    return count

def c_S(node,mark='S'):
    S = 0
    for x in node[mark]:
        if len(x) ==2:
            lS = c_S(Parser.table[x[0][1]][x[0][2]],x[0][0])
            rS = c_S(Parser.table[x[1][1]][x[1][2]],x[1][0])
            S = S + lS * rS
        else:
            S = 1
    return S        
    
            
def Analyze_Tree(parser):
    print "#S:",c_S(parser.table[0][len(parser.words)])
    print "#edge:",c_edge(parser.table)
    print "probability", math.exp(parser.table[0][len(parser.words)]['S'][0].prob)
    start = parser.table[0][len(parser.words)]
    i = 0
    for x in mktree(start,'S'):
        i += 1
        print "#",i,"Tree"
        print x


if __name__ == '__main__':
    L=lang("./data/rules_prob.txt")
    L.make_cnfs()
    if len(sys.argv) == 1:
        print "please use ./parser.py hoge hoge fuga fuga"
        print "follow is the example of Parsing the sentence \"book the flight through Houston\""
        print
        Parser = CKYparser(['book','the','flight','through','Houston'],L)
    else:
        print sys.argv[1:]
        Parser = CKYparser(sys.argv[1:],L)
    Parser.mktable()
    Analyze_Tree(Parser)
