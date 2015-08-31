
# -*- coding: utf-8 -*-

import pyximport; pyximport.install(pyimport = True)
from collections import deque

class EarleyRule:
    def __init__(self, line):
        l = line.split()

        self.LHS = l[0]
        self.RHS = []
        rhs = l[1:len(l)-1]
        for r in rhs:
            self.RHS.append(r)
        self.prob = float(l[len(l)-1])

    def toRuleStr(self, lhs, rhs, prob):
        rulestr = lhs + " "
        for s in rhs:
            rulestr += s + " "
        rulestr += str(prob)
        return rulestr

    def __str__(self):
        rulestr = self.toRuleStr(self.LHS, self.RHS, self.prob)
        return rulestr

    def isTerminal(self):
        if self.RHS[0][0] == '_':
            return True
        return False

class EarleyState:
    def __init__(self, line, start = 0, dot = 0):
        l = line.split()

        self.LHS = l[0]
        self.RHS = []
        rhs = l[1:len(l)-1]
        for r in rhs:
                self.RHS.append(r)
        self.prob = float(l[len(l)-1])

        self.start = start
        self.dot = dot

    def position(self):
        return  self.dot - self.start # 部分木上でのドットの相対位置

    def toStr(self, lhs, rhs, prob):
        p = self.position()
        rulestr = lhs + " "
        for i,s in enumerate(rhs):
            if i == p:
                rulestr += "." + " " + s + " "
            else:
                rulestr += s + " "
        if p == len(rhs):
            rulestr += "." + " "

        rulestr += str(prob)
        rulestr += " [%d,%d]" % (self.start, self.dot)
        return rulestr

    def __str__(self):
        rulestr = self.toStr(self.LHS, self.RHS, self.prob)
        return rulestr

    # ドットの右側にトークンが存在しない場合(＝それ以上展開できない場合)
    def isComplete(self):
        if self.position() >= len(self.RHS):
            return True
        return False



class Earley:
    # ckyの表は左三角行列（右上に'S', 左端に各単語が来る）
    def __init__(self):
        self.rules = []
        self.sentence = []
        self.chart = []
        #self.statequeue = deque()

    def parse(self, ori_sentence):
        self.sentence.clear()
        self.chart.clear()

        sentence = ori_sentence.split()
        for w in sentence: 
            self.sentence.append('_' + w.lower()) 


        for i in range(len(self.sentence)+1):
            self.chart.append([])

        start = EarleyState("Dummy S 1.0")
        self.addState(start, 0)
        
        #for i in range(len(self.sentence)+1):
        i = 0
        for state in self.chart[i]:
            self.predictor(state)
        for state in self.chart[i]:
            self.scanner(state)

    def checkdup(self, state, index):
        for s in self.chart[index]:
            if s.LHS == state.LHS and s.RHS == state.RHS:
                return True
        return False

    def predictor(self, state):
        if state.isComplete():
            return

        for rule in self.rules:
            if rule.isTerminal():
                continue
            if rule.LHS == state.RHS[state.position()]:
                new_state_str = rule.toRuleStr(rule.LHS, rule.RHS, rule.prob * state.prob)
                new_state = EarleyState(new_state_str, state.dot, state.dot)
                self.addState(new_state, state.dot)

    def scanner(self, state):
        if state.isComplete():
            return
        
        for rule in self.rules:
            if not rule.isTerminal():
                continue
            if rule.LHS == state.RHS[state.position()]:
                new_state_str = rule.toRuleStr(rule.LHS, rule.RHS, rule.prob * state.prob)
                new_state = EarleyState(new_state_str, state.dot, state.dot+1)
                self.addState(new_state, state.dot+1)

        pass
    def completer(self, state, index):
        if state.isComplete():
            return
        #new_state_str = 
        #new_state = EarleyState(new_state_str, state.start, state.dot+1)
        #self.addState(new_state, index)


    def addState(self, state, index):
        #重複チェック
        if not self.checkdup(state, index):
            self.chart[index].append(state)
            #self.statequeue.append(state)

    def addRule(self, rule):
        self.rules.append(rule)

    def showRules(self):
        for r in self.rules:
            print(r)

    def showStates(self):
        for i,c in enumerate(self.chart):
            print("< chart[%d] >" % i)
            for j in c:  
                print(j)


    def readCFGRules(self, filename):
        try:
            f = open(filename,"r")
        except FileNotFoundError:
            print ("Not Found: " + filename)
            exit(1)

        line = f.readline()
        while line:
            rule = EarleyRule(line)
            self.addRule(rule)
            line = f.readline()
        f.close()
