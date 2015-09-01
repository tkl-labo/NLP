
# -*- coding: utf-8 -*-

import pyximport; pyximport.install(pyimport = True)

from enum import Enum
class RuleType(Enum):
    TerminalUnary = 'TerminalUnary'
    NonTerminalUnary = 'NonTerminalUnary'
    Binary = 'Binary'
    OverTrinary = 'OverTrinary'

class PCFGRule:
    def __init__(self, line):
        l = line.split()

        self.LHS = l[0]
        self.RHS = []
        rhs = l[1:len(l)-1]
        for r in rhs:
                self.RHS.append(r)
        self.prob = float(l[len(l)-1])

        #非終端記号は '_小文字' の形式で
        if len(self.RHS) == 1 and self.RHS[0][0] == '_':
            self.ruletype = RuleType.TerminalUnary
        elif len(self.RHS) == 1:
            self.ruletype = RuleType.NonTerminalUnary
        elif len(self.RHS) == 2:
            self.ruletype = RuleType.Binary
        else:
            self.ruletype = RuleType.OverTrinary


    def toRuleStr(self, lhs, rhs, prob):
        rulestr = lhs + " "
        for s in rhs:
            rulestr += s + " "
        rulestr += str(prob)
        return rulestr

    def __str__(self):
        rulestr = self.toRuleStr(self.LHS, self.RHS, self.prob)
        return rulestr


    def toCNF(self):
        lhs = self.LHS
        rhs = list(self.RHS)
        prob = self.prob
        rules = self.decompose(lhs, rhs, prob)
        return rules

    def decompose(self, lhs, rhs, prob):
        if len(rhs) <= 2:
            rulestr = self.toRuleStr(lhs, rhs, prob)
            return [PCFGRule(rulestr)]
        else:
            r1 = "@"
            r2 = rhs.pop()
            for r in rhs:
                r1 += r + "_"
            rulestr = self.toRuleStr(lhs,[r1, r2], prob)
            rule =  PCFGRule(rulestr)

            # 一番初めに左展開する部分だけ確率を残し、その下の子については1.0
            rules = [rule] + self.decompose(r1, rhs, 1.0)
            return rules

    #ルールを適用して、左辺に生成されるものをキーとしたハッシュを返す
    def apply(self, symbols, left = None, right = None, prob = 1.0):
        if self.RHS != symbols:
            return {}
        else:
            if self.ruletype == RuleType.TerminalUnary:
                return {self.LHS: {'left': left, 'right': right, 'prob': self.prob * prob}}
            elif self.ruletype == RuleType.NonTerminalUnary:
                return {self.LHS: {'left': left, 'right': right, 'prob': self.prob * prob}}
            elif self.ruletype == RuleType.Binary:
                return {self.LHS: {'left': left, 'right': right, 'prob': self.prob * prob}}


class PCFG:
    # ckyの表は左三角行列（右上に'S', 左端に各単語が来る）
    def __init__(self):
        self.rules = []
        self.cky = None
        self.sentenceKey = 'S'
        self.sentence = []

    def isParsed(self):
        l = len(self.cky)
        return True if self.sentenceKey in self.cky[l-1][0] else False

    def showTrees(self):
        print (self.sentence)
        root = (len(self.cky)-1, 0, 'S')
        self.trace(node = root, depth = 0)
        

    def trace(self, node, depth):
        if not self.isParsed():
            print("No parsed sentence")
            return 
        if node == None:
            return
        
        (x, y, token) = node

        cell = self.cky[x][y][token]

        l = cell['left']
        r = cell['right']
        prob = cell['prob']

        if token == 'S':
            print("Prob: " + str(prob))

        if x == 0 and l == None:
            print("   " * depth + token + ")" + "   " + self.sentence[y])
        else:
            print("   " * depth + token + ")")

        if l == r:
            self.trace(l, depth+1)
        else:
            self.trace(l, depth+1)
            self.trace(r, depth+1)


    def addRule(self, rule):
        self.rules += rule.toCNF() # 3項以上のルールをCNFに変換, 非終端記号to非終端記号のUnaryなルールは別に対処

    def showRules(self):
        for r in self.rules:
            print(r)
  

    def readCFGRules(self, filename):
        try:
            f = open(filename,"r")
        except FileNotFoundError:
            print ("Not Found: " + filename)
            exit(1)

        line = f.readline()
        while line:
            rule = PCFGRule(line)
            self.addRule(rule)
            line = f.readline()
        f.close()
        #self.showRules()

    def showCkyMap(self):
        if self.cky == None:
            return
        l = len(self.cky)
        for i in range(l):
            for j in range(l):
                print "[%s][%s]" % (i,j)
                print(self.cky[i][j])


    def parse(self, ori_sentence):
        self.sentence = ori_sentence.split()
        sentence = []
        for w in self.sentence: 
            sentence.append('_' + w.lower()) 

        l = len(sentence)
        ckymap = [[{} for i in range(l)] for j in range(l)]

        #終端記号の変換
        terminal_unary_rules = [r for r in self.rules if r.ruletype == RuleType.TerminalUnary]
        for (j, symbol) in enumerate(sentence):
            for rule in terminal_unary_rules:
                ckymap[0][j].update(rule.apply([symbol]))


        for i in range(l):
            for j in range(l-i):
                self.applyNonTerminalUnaryRules(ckymap, i, j)
                self.applyBinaryRules(ckymap, i, j)
        self.applyNonTerminalUnaryRules(ckymap, l-1, 0)
        self.cky = ckymap
        #self.showCkyMap()
      
    #単語を非終端記号に変えるフェーズ。
    #def applyTerminalUnaryRules(self, symbol):
    #    unary_rules = [r for r in self.rules if r.ruletype == RuleType.TerminalUnary]
    #    for rule in unary_rules:
    #        return rule.apply(symbol)

    #非終端記号のUnaryルールを各マスごとに最大何回まで適用するか？
    def applyNonTerminalUnaryRules(self, ckymap, i, j, TH=2):
        unary_rules = [r for r in self.rules if r.ruletype == RuleType.NonTerminalUnary]
        RHS = list(ckymap[i][j].keys())
        
        changed = True
        for t in range(TH):
            if changed == False: #前回のループで何も変わらなかったら打ち切り
                break
            changed = False
            for symbol in RHS:
                for rule in unary_rules:
                    applied = rule.apply([symbol], left=(i,j,symbol), right=(i,j,symbol), prob=ckymap[i][j][symbol]['prob'])
                    if len(applied) > 0:
                        changed = True
                        for ap_key in applied.keys():
                            self.updateCKY(ckymap,i, j, ap_key, applied)
                  
    def applyBinaryRules(self, ckymap, i, j):
        if i == 0:
            return
        binary_rules = [r for r in self.rules if r.ruletype == RuleType.Binary]
        l = len(ckymap)
        for rule in binary_rules:
            for h in range(i):
                left = ckymap[h][j]
                right = ckymap[i-1-h][j+h+1]
                for lk in left.keys():
                    for rk in right.keys():
                        applied = rule.apply([lk, rk], left=(h, j, lk), right=(i-1-h, j+h+1, rk), prob=left[lk]['prob'] * right[rk]['prob'])
                        if len(applied) > 0:
                            for ap_key in applied.keys():
                                self.updateCKY(ckymap,i, j, ap_key, applied)

    #非終端記号にルール適用後、すでにそのマスに存在していた場合は確率の大きな方を取る
    def updateCKY(self, ckymap, i, j, ap_key, applied):
        if not ap_key in ckymap[i][j]:
            ckymap[i][j].update(applied)
        elif applied[ap_key]['prob'] > ckymap[i][j][ap_key]['prob']:
            ckymap[i][j].update(applied)
 
