from grammer import *
class CKYparser:
    def __init__(self,words,grammer):
        self.words = words
        self.grammer = grammer
        self.table = [['']*(len(words)+1) for i in xrange((len(words)))]
    def mktable(self):
        for i in xrange(len(self.words)):
            for j in xrange(i+1):
                if j == 0:
                    self.table[i][i+1] = self.grammer.taglist(self.words[i])
                else:
                    taglist = []
                    for k in xrange(j):
                        for tagA in self.table[i-j][i-k]:
                            for tagB in self.table[i-k][i+1]:
                                taglist.extend(self.grammer.search_gram(tagA,tagB))
                    self.table[i-j][i+1] = tuple(set(taglist))

if __name__ == '__main__':
    L = lang('/home/kawamoto/NLP/parsing/kawamoto/data/lexicon.csv','/home/kawamoto/NLP/parsing/kawamoto/data/grammer.csv')
    L.make_cnfs()
    Parser = CKYparser(['book','the','flight','through','Houston'],L)
    Parser.mktable()
    for i in Parser.table:
        for j in i:
            print j,'\t|',
