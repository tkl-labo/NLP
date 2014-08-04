#!/usr/bin/env pypy
# -*- coding:utf-8 -*-
import sys,os
#import math,re,datetime

def dataFetcher(filename):
    f = open(filename,"r")
    while True:
        words_1sentence = []
        postag_1sentence = []

        for line in f:
            if line.strip() != "":
                items = line.strip().split(" ")
                words_1sentence.append(items[0])
                postag_1sentence.append(items[1])
            else:
                words_1sentence.append(None)
                postag_1sentence.append("</s>")
                yield words_1sentence,postag_1sentence
                words_1sentence = []
                postag_1sentence = []
        raise StopIteration

def supervised_train():
    ## pre_learning_the_number_of_postag
    postaglist = list()
    f_postag = open("train_postag","r")
    for line in f_postag:
        postaglist.append(line.strip())
    postaglist.append("</s>")
    K = len(postaglist)
    print "K, the number of postag is : ",K

    ## pre_learning_the_number_of_words
    wordslist = list()
    f_words = open("train_words","r")
    for line in f_words:
        try:
            items = line.strip().split(" ")
            counts = int(items[0])
            word = items[1]
            if counts > 1: # the words whose counts = 1 is regard as UNK words
                wordslist.append(word)
        except: # word = 空白の場合
            continue 
    wordslist.append("<UNK>")
    V = len(wordslist) # including UNK words
    print "V, the number of words is : ",V

    ## initialize A, PI, B
    A_counts = [[0 for i in range(K)] for j in range(K)] 
    A = [[0 for i in range(K)] for j in range(K)]
    PI_counts = [0 for i in range(K)]
    PI = [0 for i in range(K)]
    
    B_counts = [[0 for i in range(V)] for j in range(K)]
    B = [[0 for i in range(V)] for j in range(K)]

    ## learning phase
    f_learn = "ishiwatari_data/train.txt"

    for words_1sentence,postag_1sentence in dataFetcher(f_learn):
        pre_postag = ""
        for word,postag in zip(words_1sentence,postag_1sentence):
            # get index
            if word != None: # NOT the head or end of the file
                try:
                    word_index = wordslist.index(word)
                except:
                    word_index = wordslist.index("<UNK>")
            postag_index = postaglist.index(postag)

            # train A (trainsition probability matrix) or PI (initial probability)
            if pre_postag == "":
                PI_counts[postag_index] += 1
            else:
                pre_postag_index = postaglist.index(pre_postag)
                A_counts[pre_postag_index][postag_index] += 1

            # train B (observation likelihoods)
            if postag != "</s>":
                B_counts[postag_index][word_index] += 1
        
            # prepare for the next loop
            pre_postag = postag

    ## calcuration probability
    ### PI
    print "PI, initial probability"
    for i in range(K):
        PI[i] = float(PI_counts[i] + 1) / (sum(PI) + K) # Laplace smoothing
        print postaglist[i], PI[i],  PI_counts[i],sum(PI_counts)

    ### A
    print "A, transition probability matrix"
    for i in range(K):
        for j in range(K):
            A[i][j] = float(A_counts[i][j] + 1) / (sum(A_counts[i]) + K) # Laplace smoothing
            print postaglist[i],postaglist[j],A[i][j],  A_counts[i][j],sum(A_counts[i])

    ### B
    print "B, observation likelihoods"
    for i in range(K):
        for j in range(V):
            B[i][j] = float(B_counts[i][j] + 1) / (sum(B_counts[i]) + V ) # Laplace smoothing and V include UNK
            print postaglist[i],wordslist[j],B[i][j], B_counts[i][j],sum(B_counts[i])
                            
    
    return postaglist,K,wordslist,V,A,PI,B

def calc_likelihood_by_forwarding(testfilename):
    postaglist,K,wordslist,V,A,PI,B = supervised_train()
    
    for words_1sentence,postag_1sentence in dataFetcher(testfilename):
        likelihood = 1.0
        pre_postag = ""
        for i,(word,postag) in enumerate(zip(words_1sentence,postag_1sentence)):
            postag_index = postaglist.index(postag)
            
            if i == 0: #the head of sentence
                likelihood *= PI[postag_index]
            elif postag == "</s>": #the tail of sentence
                likelihood *= A[pre_postag_index][postag_index]
            else:
                try:
                    word_index = wordslist.index(word)
                except:
                    word_index = wordslist.index("<UNK>")
                likelihood *= A[pre_postag_index][postag_index] * B[postag_index][word_index]

            pre_postag = postag
            pre_postag_index = postaglist.index(pre_postag)

        #yield " ".join(words_1sentence),likelihood
        print " ".join(words_1sentence[:-1]),likelihood

    #raise StopIteration

if __name__ == "__main__":
    #for word_s,postag_s in dataFetcher("ishiwatari_data/mini_train"):
    #    print word_s
    #    print postag_s,"\n"
    #supervised_train()
    calc_likelihood_by_forwarding("ishiwatari_data/mini_test")
