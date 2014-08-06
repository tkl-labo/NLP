#HMM POS tagger

##hmm.py

####supervised_train()
	training HMM variables(PI,A,B,and so on) with Laplace smoothing

####calc_likelihood(testfilename)
	calculating likelihood of the sentence given word and POStag

####decoding(testflename)
- read a testfile
- find the most probable postag sequences on each sentence

```
$ ./hmm.py
Pickle load:  hmm.pickle

Sentence:  I like summer very much .
 Prob_of_max_prob_path:  4.50288468846e-21
 Best_track:  [24, 13, 18, 26, 14, 6]
 Viterbi POSTAGGING RESURT
        word         predPOS ansPOS 
        ------------ ------- -------
        I            PRP     PRP    
        like         IN      VB     
        summer       NN      NN     
        very         RB      RB     
        much         JJ      RB     
        .            .       .   

```

At the end of prediction, decoding() outputs

- evaluation of postagging accuracy
- simple error analysis

```
All POStag prediction has done!
        The NUM of sentence:  2012
        The NUM of tokens:  47377
        Process time: 8.36s
        The NUM of correct prediction : 42624/47377
        Accuracy: 89.968%

        The TOP 10 failure of POStagging
        rank   predPOS  ansPOS   counts
        1       NN      NNS     274
        2       NNP     NN      246
        3       JJ      NN      235
        4       NNP     NNS     193
        5       JJ      NNP     185
        6       NN      NNP     143
        7       NNP     JJ      141
        8       JJ      CD      117
        9       NN      JJ      87
        10      NNP     NNPS    72
```

This tagger processed 240 sentences (5,667 tokens) / sec. on Linux server with ?? @ ?? GHz