#!/usr/bin/env pypy
# -*- coding:utf-8 -*-
import sys,os
import math,re,datetime,time
import pickle
from collections import defaultdict as dd

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
    f_learn = "../data/ishiwatari_data/train.txt"

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
        PI[i] = float(PI_counts[i] + 1) / (sum(PI_counts) + K) # Laplace smoothing
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
               
    save( (postaglist,K,wordslist,V,A,PI,B),"hmm.pickle")
    
    return postaglist,K,wordslist,V,A,PI,B

def save(datatuple,filename):
    f = open(filename,"wb")
    for data in datatuple:
        pickle.dump(data,f)
    f.close()
    print "Pickle dump: ",filename
    
def load(filename):
    f = open(filename,"rb")
    datalist = []
    while True:
        try:
            datalist.append(pickle.load(f))
        except EOFError:
            break
    f.close()
    print "Pickle load: ",filename
    return tuple(datalist)

def calc_likelihood(testfilename):
    #postaglist,K,wordslist,V,A,PI,B = supervised_train()
    postaglist,K,wordslist,V,A,PI,B = load("hmm.pickle")
    
    for words_1sentence,postag_1sentence in dataFetcher(testfilename):
        log_likelihood = 0.0
        pre_postag = ""
        for i,(word,postag) in enumerate(zip(words_1sentence,postag_1sentence)):
            postag_index = postaglist.index(postag)
            
            if i == 0: #the head of sentence
                try:
                    word_index = wordslist.index(word)
                except:
                    word_index = wordslist.index("<UNK>")

                log_likelihood += math.log(PI[postag_index]) + math.log(B[postag_index][word_index])
            elif postag == "</s>": #the tail of sentence
                log_likelihood += math.log(A[pre_postag_index][postag_index])
            else:
                try:
                    word_index = wordslist.index(word)
                except:
                    word_index = wordslist.index("<UNK>")

                log_likelihood += math.log(A[pre_postag_index][postag_index]) +math.log(B[postag_index][word_index])

            pre_postag = postag
            pre_postag_index = postaglist.index(pre_postag)

        likelihood = math.exp(log_likelihood)

        print likelihood

def decoding(testfilename):
    postaglist,K,wordslist,V,A,PI,B = load("hmm.pickle")

    start_time = time.clock()
    sentence_num = 0
    token_num = 0
    correct_num = 0
    fail_to_predict_postag_pair = dd(int)
    
    for words_1sentence,postag_1sentence in dataFetcher(testfilename):
        print "\nSentence: "," ".join(words_1sentence[:-1])
        backpointers = [[None for i in range(K)] for j in range(len(words_1sentence)-1)]
        viterbi = [[0 for i in range(K)] for j in range(len(words_1sentence))]

        sentence_num += 1
        token_num += len(words_1sentence) -1

        for i,word in enumerate(words_1sentence):

            if i == 0: #the head of sentence
                try:
                    word_index = wordslist.index(word)
                except:
                    word_index = wordslist.index("<UNK>")

                for k in range(K):
                    backpointers[0][k] = "<s>"

                for k in range(K):
                    viterbi[0][k] = PI[k] * B[k][word_index]


            elif i == (len(words_1sentence) -1): #the end of sentence
                prob = []
                postag_index = postaglist.index("</s>")
                for k in range(K):
                    p = viterbi[i-1][k] * A[k][postag_index]
                    prob.append(p)
                else:
                    sentence_maxprob = max(prob)
                    first_backpointer = prob.index(max(prob))

                    print " Prob_of_max_prob_path: ",sentence_maxprob
                    best_track = backtrack(backpointers,first_backpointer)
                    print " Best_track: ",best_track
                    sentence_postag = []
                    for track in best_track: # remove <s> postag
                        sentence_postag.append(postaglist[track])

                    print " Viterbi POSTAGGING RESURT"
                    print "\t{:<12} {:<7} {:<7}".format("word","predPOS","ansPOS")
                    print "\t{:<12} {:<7} {:<7}".format("-"*12,"-"*7,"-"*7)
                    for word,predict_postag,answer_postag in zip(words_1sentence[:-1],sentence_postag,postag_1sentence[:-1]):
                        print "\t{:<12} {:<7} {:<7}".format(word,predict_postag,answer_postag)
                        if predict_postag == answer_postag:
                            correct_num += 1
                        else:
                            fail_to_predict_postag_pair[(predict_postag,answer_postag)] += 1

            else: # NEITHER the head nor end of the sentence
                try:
                    word_index = wordslist.index(word)
                except:
                    word_index = wordslist.index("<UNK>")
                
                for k in range(K):
                    prob = []
                    for l in range(K):
                        p = viterbi[i-1][l] * A[l][k] * B[k][word_index]
                        prob.append(p)
                    else:
                        viterbi[i][k] = max(prob)
                        backpointers[i][k] = prob.index(max(prob))
                
    # POStagging evaluation
    print "\nAll POStag prediction has done!"
    print "\tThe NUM of sentence: ",sentence_num
    print "\tThe NUM of tokens: ",token_num
    print "\tProcess time: {:.2f}s".format(time.clock() -start_time)
    print "\tThe NUM of correct prediction : {}/{}".format(correct_num,token_num)
    print "\tAccuracy: {:.3f}%".format(float(correct_num)*100/token_num)

    # error analysis
    print "\n\tThe TOP 10 failure of POStagging"
    print "\t{:<5}  {:<7}  {:<7}  {}".format("rank","predPOS","ansPOS","counts")
    for i,((predPOS,ansPOS),counts) in enumerate(sorted(fail_to_predict_postag_pair.items(),key=lambda x :x[1],reverse=True)[0:10]):
        print "\t{}\t{}\t{}\t{}".format(i+1,predPOS,ansPOS,counts)

def backtrack(backpointers,first_backpointer):
    backpointer = first_backpointer
    best_track = [backpointer]

    #print "first_backpointer ",first_backpointer
    #print "backpointers: ",backpointers
    for backpointers_t in reversed(backpointers):
        backpointer = backpointers_t[backpointer]
        if not backpointer == "<s>":
            best_track.append(backpointer)
        else:
            break

    best_track.reverse()
    return best_track

if __name__ == "__main__":
    #for word_s,postag_s in dataFetcher("ishiwatari_data/mini_train"):
    #    print word_s
    #    print postag_s,"\n"
    #supervised_train()
    #calc_likelihood("ishiwatari_data/test.txt")
    #calc_likelihood("../data/my_test")
    #decoding("../data/my_test")
    decoding("../data/ishiwatari_data/test.txt")
