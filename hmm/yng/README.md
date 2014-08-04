A simple tagger based on first-order HMM
===
+ train.cc - learning parameters for first-order HMM
  - assume words with low frequency as unknown words
  - apply laplace smoothing for transition probabilities

            > g++ -I. -Wall -O2 -g train.cc -o train
            > zcat train.txt.gz | ./train 1 > model
            collecting statistics: ........0.058s
            # sentences: 8936
            # tags: 44
            # words: 19122
            # words (above threshold): 9674
  
+ tagger.cc - implementation of viterbi algorithm for first-order HMM
  - some room for speed-up

            > g++ -I. -Wall -O2 -g tagger.cc -o tagger
            > zcat test.txt.gz | ./tagger model
            reading model: 0.029s
            tagging words: 0.150s
            # sentences: 2012
            acc. 0.9368 (corr 44385) (incorr 2992)
           
tagger processes 13,413 sentences / sec. on Linux server with Intel(R) Xeon(R) CPU E5-2680 v2 @ 2.80GHz
