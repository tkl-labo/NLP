#!/usr/bin/env python
# -*- coding:utf-8 -*-
import sys,os
import math,re,datetime
import romkan,unicodedata

## making dictionary of probability of occurence of hiragana-word
def train(features):
    print "START TRAINING"
    worddic = dict()
    for line in features:
        items = line.strip().split("\t")
        string = items[0].decode("utf8")
        string_roma = unicodedata.normalize("NFKC",romkan.to_roma(string))
        freq = float(items[1])
        worddic[string_roma] = freq
        if len(worddic) % 10000 == 0:
            print len(worddic)
    print "FINISH TRAINING"
    return worddic

WORDDIC = train(open("../ngram_data/kana_freq.tsv","r"))
alphabet = "abcdefghijklmnopqrstuvwxyz"

## the probability of mistype
EDIT1_PROB = 0.15
EDIT2_PROB = 0.05

def edit1_candidate(word):
    n = len(word)
    e1set = set(#[word[0:i] + word[i+1:] for i in range(n)] + #deletion 
                [word[0:i] + c + word[i:] for i in range(n+1) for c in alphabet] +  #insertion
                [word[0:i] + word[i+1] + word[i] + word[i+2:] for i in range(n-1)])  #transposition
    return e1set
            

def edit2_candidate(word):
    n = len(word)
    e2set = set([word[0:i] + c + word[i+1:] for i in range(n+1) for c in alphabet]) #alteration
    for e1 in edit1_candidate(word):
        for e2 in edit1_candidate(e1):
            e2set.add(e2)
    return e2set

def prob(word):
    if word in WORDDIC:
        return [(word,WORDDIC[word])]
    else:
        return []

def edit1_prob(word):
    tmp = []
    for word_e1 in edit1_candidate(word):
        if word_e1 in WORDDIC:
            tmp.append((word_e1,EDIT1_PROB*WORDDIC[word_e1]))
    return tmp

def edit2_prob(word):
    tmp = []
    for word_e2 in edit2_candidate(word):
        if word_e2 in WORDDIC:
            tmp.append((word_e2,EDIT2_PROB*WORDDIC[word_e2]))
    return tmp

## printing candidate correct word (1~num)
def hiragana_candidates(word,num):
    if not isinstance(word,unicode): #unicode check
        word = word.decode("utf8")

    romaji = unicodedata.normalize("NFKC",romkan.to_roma(word))
    print "romaji:{}".format(romaji)

    candidates = prob(romaji) + edit1_prob(romaji) + edit2_prob(romaji)
    if candidates:
        for i,word_prob_tuple in enumerate(sorted(candidates,key=lambda x :x[1],reverse=True)[:num]):
            romaji = word_prob_tuple[0]
            p = word_prob_tuple[1]
            kana = romkan.to_hiragana(romaji).encode("utf8")
            print " {} : {:<10}{:<20} {:<}".format(i,kana,"("+romaji+")",p)
    else:
        print "NO RESULT"

def test(testset):
    for string,correct in testset.items():
        print "\n{}, correct:{}".format(string.encode("utf8"),correct.encode("utf8"))
        hiragana_candidates(string,3)    

if __name__ == "__main__":
    test1 = {u"けいたいｄんわ":u"けいたいでんわ",
             u"つらぴょ":u"つらぽよ",
             u"まじきょ":u"まじかよ",
             u"ぽけん":u"ポケモン",
             u"うそｄｓ":u"うそだ",
             u"ほんたの":u"ほんとなの",
             u"ゆうおｋりん":u"ゆうこりん",
             u"でｈそ":u"でしょ",
             u"りらｋま":u"りらっくま",
             u"いてもあｗ":u"?",
             u"いｔｓも":u"いつも",
             u"なんｄあｙろ":u"なんでやろ"}

    if len(sys.argv) == 2:
        word = sys.argv[1]
        hiragana_candidates(word,3)
    elif len(sys.argv) == 1:
        test(test1)
