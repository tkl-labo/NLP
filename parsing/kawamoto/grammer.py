import csv
from sets import Set
class lang:
    def __init__(self,lexicon_file,grammer_file):
        #input grammer and lexicon
        f = open(lexicon_file,'rb')
        reader = csv.reader(f)
        self.lexicon={ x[0]:x[1:] for x in reader}
        self.terminal = self.lexicon.keys()
        self.non_terminal=Set()
        f = open(grammer_file,'rb')
        reader = csv.reader(f)
        self.cfgs = []
        for i in reader:
            self.cfgs.append((i[0], tuple(i[1:])))
            for sig in i:
                if self.is_terminal(sig) != True:
                    self.non_terminal.add(sig)
        self.non_terminal=list(self.non_terminal)
    def make_cnfs(self):
        self.cnfs_lexicon = self.lexicon
        self.cnfs = []
        for cfg in self.cfgs:
            self.cnfs.extend(self.cfg2cnf(cfg))
    def is_terminal(self, sig):
        return self.lexicon.has_key(sig)
    def cfg2cnf(self,cfg):
        cnfs = []
        if (len(cfg[1]) > 2):
            index = 0
            self.non_terminal.append(len(self.non_terminal))
            cnfs.append((cfg[0],(cfg[1][index],len(self.non_terminal)-1)))
            index += 1
            while(len(cfg[1])-index>2):
                self.non_terminal.append(len(self.non_terminal))
                cnfs.append((len(self.non_terminal-1),(cfg[1][index],len(self.non_terminal)-1)))
                index += 1
            cnfs.append((len(self.non_terminal)-1,(cfg[1][index],cfg[1][index+1])))
        elif(len(cfg[1]) == 2):
            cnfs.append(cfg)
        else:
            if self.is_terminal(cfg[1][0]):
                self.cnfs_lexicon[cfg[0]]=self.cnfs_lexicon[cfg[1][0]]
            for i in self.cfgs:
                if cfg[1][0]==i[0]:
                    cnfs.extend( self.cfg2cnf((cfg[0],i[1]) ))
        return cnfs
    def taglist(self,word):
        taglist = []
        for x,y in self.lexicon.items():
            if y.count(word) > 0:
                taglist.append(x)
        return tuple(taglist)
    def search_gram(self,tagA,tagB):
        taglist = []
        for x,y in self.cnfs:
            if y==(tagA,tagB):
                taglist.append(x)
        return taglist

if __name__ == '__main__':
    L = lang('/home/kawamoto/NLP/parsing/kawamoto/data/lexicon.csv','/home/kawamoto/NLP/parsing/kawamoto/data/grammer.csv')
    L.make_cnfs()
    for x in L.cnfs:
        print x[0],x[1]


