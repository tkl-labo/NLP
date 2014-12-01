from grammer import *
import sys
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
#                        print taglist
                        self.table[i-j][i+1] = taglist
                    else:
                        pass

def go_branch(node,mark):
    edge = 0
    S = 0
    print mark,
    for x in node[mark]:
        if len(x) ==2:
            print '(',
            le, lS = go_branch(Parser.table[x[0][1]][x[0][2]],x[0][0])
            re, rS = go_branch(Parser.table[x[1][1]][x[1][2]],x[1][0])
            print ')',
            edge = edge + le + re + 2
            S = S + lS * rS
        else:
            print x[0],
            S = 1
        if mark == 'S':
            print
    return edge, S

def Analyze_Tree(parser):
    start = parser.table[0][len(parser.words)]
    edge, sent = go_branch(start,'S')
    print "edge : ",edge,"sent : ",sent

if __name__ == '__main__':
    L = lang('/home/kawamoto/NLP/parsing/kawamoto/data/lexicon.csv','/home/kawamoto/NLP/parsing/kawamoto/data/grammer.csv')
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
