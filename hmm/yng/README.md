A simple tagger based on first-order HMM
===
+ train.cc - learning parameters for first-order HMM
 - assume words with low frequency as unknown words
 - apply laplace smoothing for transition probabilities

            > g++ -I. -Wall -O2 -g train.cc -o train
            > zcat train.txt.gz | ./train 10 > model
            collecting statistics: ........0.060s
            # sentences: 8936
            # tags: 44
            # words: 19122
            # words (above threshold): 2251
  
+ tagger.cc - implementation of viterbi algorithm for first-order HMM
 - some room for speed-up

            > g++ -I. -Wall -O2 -g tagger.cc -o tagger
            > zcat test.txt.gz | ./tagger model
            reading model: 0.063s
            tagging words: ..0.130s
            # sentences: 2012
            acc. 0.9510 (corr 45054) (incorr 2323)

tagger processes 15,476 sentences / sec. on Linux server with Intel(R) Xeon(R) CPU E5-2680 v2 @ 2.80GHz
