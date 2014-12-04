from grammer import *
import sys
import itertools
class CKYparser:
    def __init__(self,words,grammer):
        self.words = words
        self.grammer = grammer
        self.table = [[{}]*(len(words)+1) for i in xrange((len(words)))]
    def mktable(self):
        for i in xrange(len(self.words)):
            for j in xrange(i+1):
                if j == 0:
                    self.table[i][i+1] = {x:[(self.words[i],i,i+1)] for x in self.grammer.taglist(self.words[i]) }
                else:
                    taglist = {}
                    for k in xrange(j):
                        if self.table[i-j][i-k] =='' or self.table[i-k][i+1] =='':
                            continue
                        for tagA in self.table[i-j][i-k]:
                            for tagB in self.table[i-k][i+1]:
                                for x in self.grammer.search_gram(tagA,tagB):
                                    if taglist.has_key(x):
                                        taglist[x].append(((tagA,i-j,i-k),(tagB,i-k,i+1)))
                                    else:
                                        taglist.update( { x:[((tagA,i-j,i-k),(tagB,i-k,i+1))]}) 
                    if(len(taglist)):
                        self.table[i-j][i+1] = taglist
                    else:
                        pass


def mktree(node,mark):
    for x in node[mark]:
        if len(x) ==2:
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
    start = parser.table[0][len(parser.words)]
    i = 0
    for x in mktree(start,'S'):
        i += 1
        print "#",i,"Tree"
        print x


if __name__ == '__main__':
    L = lang('./data/rules.txt')
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
