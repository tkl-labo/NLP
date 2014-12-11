import csv
from sets import Set
from collections import defaultdict
from collections import namedtuple
Prob_rule = namedtuple('Prob_rule',['Left','Right','Prob'])
class lang:
    def __init__(self,rule_file):
        #input grammer and lexicon
        f = open(rule_file,'rb')
        reader = csv.reader(f,delimiter=' ')
        self.lexicon=defaultdict(list)
        self.cfgs = []
        for row in reader:
            if row[1][0] == '_':
                self.lexicon[row[0]].append(row[1])
#            else:
            self.cfgs.append(Prob_rule(row[0],tuple(row[1:-1]),float(row[-1])))
        self.terminal = self.lexicon.keys()
        self.non_terminal=Set()
        for i in self.cfgs:
             self.non_terminal.add(i[0])
             for j in i[1]:
                 self.non_terminal.add(j)
        self.non_terminal=list(self.non_terminal)
    def make_cnfs(self):
        self.cnfs_lexicon = self.lexicon.copy()
        self.cnfs = []
        self.temp_gram = []
        for cfg in self.cfgs:
            self.cnfs.extend(self.cfg2cnf(cfg))
        self.cnfs.extend(self.temp_gram)
        self.cnfs.extend([x for x in self.cfgs if x[1][0][0]=='_'])
    def is_terminal(self, sig):
        return self.lexicon.has_key(sig)
    def cfg2cnf(self,cfg):
        cnfs = []
        if (len(cfg[1]) == 3):
            if( self.search_gram(cfg[1][0],cfg[1][1],self.temp_gram) ==[] ):
#                print cfg
                self.non_terminal.append(len(self.non_terminal))
                self.temp_gram.append( Prob_rule(cfg[0],(len(self.non_terminal),cfg[1][2]),cfg.Prob) )
                self.temp_gram.append( Prob_rule(len(self.non_terminal),(cfg[1][0],cfg[1][1]),1) )
            else:
#                print cfg,self.search_gram(cfg[1][0],cfg[1][1],self.temp_gram)[0],cfg[1][2]
                cnfs.append(Prob_rule(cfg[0],(self.search_gram(cfg[1][0],cfg[1][1],self.temp_gram)[0],cfg[1][2]),cfg.Prob))
        elif(len(cfg[1]) == 2):
            cnfs.append(cfg)
        elif(len(cfg[1]) == 1):
            if self.is_terminal(cfg[1][0]):
                if self.cnfs_lexicon.has_key(cfg[0]):
                    self.cnfs_lexicon[cfg[0]].extend(self.cnfs_lexicon[cfg[1][0]])
                else:
                    self.cnfs_lexicon[cfg[0]]=self.cnfs_lexicon[cfg[1][0]][:]
            for i in self.cfgs:
                if cfg[1][0]==i[0]:
#                    print cfg,i
                    cnfs.extend( self.cfg2cnf(Prob_rule(cfg[0],i[1],cfg.Prob*i.Prob) ))
                if len(i[1])==1 and i[1][0][0]=='_' and cfg[1][0] == i[0]:
                    cnfs.append( Prob_rule(cfg[0],(i[1][0],),cfg.Prob*i.Prob))
        else:
            self.non_terminal.append(len(self.non_terminal))
            cnfs.append( Prob_rule(cfg[0],(len(self.non_terminal),cfg[1][len(cfg[1])-1]),Prob=cfg.Prob) )
#            print 'aaaa', cfg[1][:-1]
            cnfs.extend( self.cfg2cnf(cfg[1][:-1]) )
        return cnfs
    def taglist(self,word):
        taglist = []
        for x,y in self.cnfs_lexicon.items():
            if y.count(word) > 0:
                taglist.append(x)
        return tuple(taglist)
    def search_gram(self,tagA,tagB, cnfs=None):
        taglist = []
        if cnfs==None:
            cnfs = self.cnfs
#        print cnfs
        for x,y,prob in cnfs:
            if y==(tagA,tagB):
                taglist.append(x)
        return taglist
    def calc_prob(self,left,right):
        ret=0
        for x,y,prob in self.cnfs:
            if x==left and y==right:
                ret = prob
                break
        return ret



if __name__ == '__main__':
    L = lang('./data/rules_prob.txt')
    L.make_cnfs()
    for x in L.cnfs:
        print x 
    print L.calc_prob('S','_book')
