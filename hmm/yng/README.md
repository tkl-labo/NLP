A simple tagger based on first-order HMM
===
+ train.cc - learning parameters for first-order HMM
 - use tag distribtion for rare words to estimate that for unknown words
 - apply laplace smoothing for transition probabilities

            > g++ -I. -Wall -O2 -g train.cc -o train
            > zcat train.txt.gz | ./train 10 > model
            collecting statistics: ........0.058s
            # sentences: 8936
            # tags: 44
            # words: 19122
            # words (above threshold): 2251
  
+ tagger.cc - viterbi algorithm for first-order HMM
 - yet some room for speed-up

            > g++ -I. -Wall -O2 -g tagger.cc -o tagger
            > zcat test.txt.gz | ./tagger model
            reading model: 0.062s
            tagging words: ..0.131s
            # sentences: 2012
            acc. 0.9555 (corr 45270) (incorr 2107)
              (seen)   0.9576 (corr 44281) (incorr 1963)
              (unseen) 0.8729 (corr 989) (incorr 144)

tagger processes 15,358 sentences / sec. on Linux server with Intel(R) Xeon(R) CPU E5-2680 v2 @ 2.80GHz
