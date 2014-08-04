#!/usr/bin/env python
# -*- coding:utf-8 -*-
import sys,os
#import math,re,datetime

def supervised_train():

    ## pre_learning_the_number_of_postag
    postag = list()
    f_postag = open("mini_train_postag","r")
    for line in f_postag:
        postag.append(line.strip())
    postag.append("</s>")
    K = len(postag)
    print "K, the number of postag is : ",K

    ## pre_learning_the_number_of_words
    words = list()
    f_words = open("mini_train_words","r")
    for line in f_words:
        try:
            items = line.strip().split(" ")
            counts = items[0]
            word = items[1]
            if counts > 1: # the words whose counts = 1 is regard as UNK words
                words.append(word)
        except: # word = 空白の場合
            continue 
    words.append("</s>")
    words.append("<UNK>")
    V = len(words) # including UNK words


    ## initialize A, PI, B
    A_counts = [[0 for i in range(K)] for j in range(K)] 
    A = [[0 for i in range(K)] for j in range(K)]
    PI_counts = [0 for i in range(K)]
    PI = [0 for i in range(K)]
    
    B_counts = [[0 for i in range(V)] for j in range(V)]
    B = [[0 for i in range(V)] for j in range(V)]

    ## learning phase
    f_learn = open("ishiwatari_data/mini_train","r")

    pre_postag = "<s>"
    for line in f_learn:
        if line.strip() != "": # NOT the end of the sentence
            items = line.strip().split(" ")
            word = items[0]
            postag = items[1]

            try:
                word_index = words.index(word)
            except:
                word_index = words.index("<UNK>")
            postag_index = postag.index(postag)

            # train A (trainsition probability matrix) or PI (initial probability)
            if pre_postag == "<s>":
                PI_counts[postag_index] += 1
            else:
                pre_postag_index = postag.index(pre_postag)
                A_counts[pre_postag_index][postag_index] += 1

            # train B (observation likelihoods)
            B_counts[postag_index][word_index] += 1
        
            # prepare for the next loop
            pre_postag = postag

        else: # the end of the sentence
            postag = "</s>"
            postag_index = postag.index(postag)
        
            # train A (trainsition probability matrix) or PI (initial probability)
            if pre_postag == "<s>":
                PI_counts[postag_index] += 1
            else:
                pre_postag_index = postag.index(pre_postag)
                A_counts[pre_postag_index][postag_index] += 1
            

    ## calcuration probability
    ### PI
    print "PI, initial probability"
    for i in range(K):
        PI[i] = float(PI_counts[i] + 1) / (sum(PI) + K) # Laplace smoothing
        print postag[i], PI[i]

    ### A
    print "A, transition probability matrix"
    for i in range(K):
        for j in range(K):
            A[i][j] = float(A_counts[i][j] + 1) / (sum(A_counts[i]) + K) # Laplace smoothing
            print postag[i],postag[j],A[i][j]

    ### B
    print "B, observation likelihoods"
    for i in range(K):
        for j in range(V):
            B[i][j] = float(B_counts[i][j] + 1) / (sum(B_counts[i]) + V ) # Laplace smoothing and V include UNK
            print postag[i],words[j],B[i][j]
                            
    

if __name__ == "__main__":
    supervised_train()
