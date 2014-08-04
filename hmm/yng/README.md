A simple tagger based on first-order HMM
===
+ train.cc - learning parameters for first-order HMM
  - assume words with low frequency as unknown words
  - apply laplace smoothing for transition probabilities

            > g++ -I. -Wall -O2 -g train.cc -o train
            > zcat train.txt.gz | time ./train 1 > model
            # sentences: 8936
            # tags: 44
            # words: 19122
            # words (above threshold): 9674
            ./train 1 > model  0.14s user 0.01s system 98% cpu 0.155 total
  
+ tagger.cc - implementation of viterbi algorithm for first-order HMM
  - some room for speed-up

            > g++ -I. -Wall -O2 -g tagger.cc -o tagger
            > zcat test.txt.gz | time ./tagger model
            reading model..done.
            tagging words..done.
            # sentences: 2012
            acc. 0.9368 (corr 44385) (incorr 2992)
            ./tagger model  0.18s user 0.01s system 99% cpu 0.184 total

tagger processes 10,934 sentences / sec. on Linux server with Intel(R) Xeon(R) CPU E5-2680 v2 @ 2.80GHz
