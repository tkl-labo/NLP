A simple tagger based on first-order HMM
===
+ train.cc - learning parameters for first-order HMM
  - assume words with low frequency as unknown words
  - apply laplace smoothing for transition probabilities

            > g++ -I. -Wall -O2 -g train.cc -o train
            > zcat train.txt.gz | time ./train 1 > model
            # sent: 8936
            # tags: 44
            # words: 19122
            # words (above threshold): 9674
            ./train 1 > model  0.24s user 0.02s system 90% cpu 0.285 total
  
+ tagger.cc - implementation of viterbi algorithm for first-order HMM
  - some room for speed-up

            > g++ -I. -Wall -O2 -g tagger.cc -o tagger
            > zcat test.txt.gz | time ./tagger model
            reading model..done.
            tagging words..done.
            # sent: 2012
            acc. 0.9368 (corr 44385) (incorr 2992)
            ./tagger model  0.31s user 0.01s system 97% cpu 0.326 total

tagger processes 6171 sentences / sec. on Linux server with Intel Xeon X7460 @ 2.66GHz
