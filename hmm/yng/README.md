A simple POS tagger based on first-order HMM
===
+ train.cc - learn transition and emission probabilities for first-order HMM
 - estimate emission probabilities of unknown words from those of rare words, considering capitalization and suffixes
 - smooth transition probabilities by interpolation
 - support readable model output

            > g++ -I. -Wall -O2 -g train.cc -o train
            > zcat train.txt.gz | ./train 10 > model
            collecting statistics: ........0.059s
            # sentences: 8936
            # tokens: 211727
            # words: 19122
            # tags: 44
  
+ tagger.cc - find the best tag sequences using Viterbi algorithm
 - output POS-tagged words while hilighting errors
 - report accurcy for seen and unseen words
 - support quiet test (when compiled with -DNDEBUG)
 - execute beam search (when compiled with -DUSE_BEAM_SIZE=N (N=1,..))

            > g++ -DNDEBUG -I. -Wall -O2 -g tagger.cc -o tagger
            > zcat test.txt.gz | ./tagger model
            reading model: 0.059s
            tagging words: ..0.026s
            # sentences: 2012
            acc. 0.9696 (corr 45939) (incorr 1438)
             (seen)   0.9806 (corr 43219) (incorr 856)
             (unseen) 0.8237 (corr 2720) (incorr 582)
 
This tagger processes 77,384  sentences (1,822,192 tokens) / sec. on Linux server with Intel(R) Xeon(R) CPU E5-2680 v2 @ 2.80GHz
