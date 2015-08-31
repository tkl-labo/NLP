
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
    def __init__(self, line, start = 0, dot = 0, created_by = None, created_from = None):
        l = line.split()

        self.LHS = l[0]
        self.RHS = []
        rhs = l[1:len(l)-1]
        for r in rhs:
                self.RHS.append(r)
        self.prob = float(l[len(l)-1])

        self.start = start
        self.dot = dot
        self.created_by = created_by
        self.created_from = created_from


    def position(self):
        return  self.dot - self.start # 部分木上でのドットの相対位置

    def toRuleStr(self, lhs, rhs, prob):
        rulestr = lhs + " "
        for i,s in enumerate(rhs):
                rulestr += s + " "
 
        rulestr += str(prob)
        return rulestr

    def toStateStr(self, lhs, rhs, prob):
        p = self.position()
        statestr = lhs + " "
        for i,s in enumerate(rhs):
            if i == p:
                statestr += "." + " " + s + " "
            else:
                statestr += s + " "
        if p == len(rhs):
            statestr += "." + " "

        statestr += str(prob)
        statestr += " [%d,%d]" % (self.start, self.dot)
        statestr += " " + self.created_by 
        return statestr

    def __str__(self):
        statestr = self.toStateStr(self.LHS, self.RHS, self.prob)
        return statestr

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

    def parse(self, ori_sentence):
        self.sentence.clear()
        self.chart.clear()

        sentence = ori_sentence.split()
        for w in sentence: 
            self.sentence.append('_' + w.lower()) 


        for i in range(len(self.sentence)+1):
            self.chart.append([])

        start = EarleyState("Dummy S 1.0", 0, 0, "Dummy", None)
        self.addState(start, 0)
        
        for i in range(len(self.sentence)+1):
            for state in self.chart[i]:
                self.completer(state)
            for state in self.chart[i]:
                self.predictor(state)
            for state in self.chart[i]:
                self.scanner(state)
 

    def traceRoot(self):
        l = len(self.sentence) 
        for root in self.chart[l]:
            if root.LHS == 'S' and root.start == 0:
                print(root)
                self.trace(root.created_from)
                exit(1)
        

    def trace(self, states):
        if states == None:
            return
        for state in states:
            if state.LHS == "Dummy":
                continue
            print ("  " + str(state))
            for s in state.created_from:
                print ("    " + str(s))
            #if state.created_by != "Dummy":
                #self.trace(state.created_from)


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
                new_state = EarleyState(new_state_str, state.dot, state.dot, "Predictor", [state])
                self.addState(new_state, state.dot)

    def scanner(self, state):
        if state.isComplete():
            return
        
        for rule in self.rules:
            if not rule.isTerminal():
                continue
            if rule.LHS == state.RHS[state.position()]:
                new_state_str = rule.toRuleStr(rule.LHS, rule.RHS, rule.prob)
                new_state = EarleyState(new_state_str, state.dot, state.dot+1, "Scanner", [state])
                self.addState(new_state, state.dot+1)

        

    # ドットが右端まで到達したStateについて、その部分木を適用でき、ドットが対象とするLHSの左にある他のStateを探す
    def completer(self, state):
        if not state.isComplete():
            return

        for upper_state in self.chart[state.start]:
            if upper_state.isComplete():
                continue
            if state.LHS == upper_state.RHS[upper_state.position()]:
                new_state_str = upper_state.toRuleStr(upper_state.LHS, upper_state.RHS, upper_state.prob * state.prob)
                new_state = EarleyState(new_state_str, upper_state.start, state.dot, "Completer", [upper_state, state])
                self.addState(new_state, state.dot)
            
        #new_state_str = 
        #new_state = EarleyState(new_state_str, state.start, state.dot+1)
        #self.addState(new_state, index)


    def addState(self, state, index):
        #重複チェック
        if not self.checkdup(state, index):
            self.chart[index].append(state)

    def addRule(self, rule):
        self.rules.append(rule)

    def showRules(self):
        for r in self.rules:
            print(r)

    def showStates(self):
        for i,c in enumerate(self.chart):
            print("< chart[%d] >" % i)
            for j in c:  
                print("  " + str(j))


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
