#HMM POS tagger

##hmm.py

###supervised_train()
	training HMM variables(PI,A,B,and so on) with Laplace smoothing

###calc_likelihood(testfilename)
	calculating likelihood of the sentence given word and POStag

###decoding(testflename)
- read the testfile
- output the most probable postag on each sentence

```
$ ./hmm.py
Pickle load:  hmm.pickle

Sentence:  Rockwell International Corp. 's Tulsa unit said it signed a tentative agreement extending its contract with Boeing Co. to provide structural parts for Boeing 's 747 jetliners .
 Prob_of_max_prob_path:  4.58371890095e-82
 Best_track:  [19, 19, 19, 23, 14, 18, 34, 24, 34, 10, 14, 18, 34, 25, 18, 13, 19, 19, 31, 33, 14, 21, 13, 19, 23, 14, 18, 6]
 Viterbi POSTAGGING RESURT
        word         predPOS ansPOS 
        ------------ ------- -------
        Rockwell     NNP     NNP    
        International NNP     NNP    
        Corp.        NNP     NNP    
        's           POS     POS    
        Tulsa        JJ      NNP    
        unit         NN      NN     
        said         VBD     VBD    
        it           PRP     PRP    
        signed       VBD     VBD    
        a            DT      DT     
        tentative    JJ      JJ     
        agreement    NN      NN     
        extending    VBD     VBG    
        its          PRP$    PRP$   
        contract     NN      NN     
        with         IN      IN     
        Boeing       NNP     NNP    
        Co.          NNP     NNP    
        to           TO      TO     
        provide      VB      VB     
        structural   JJ      JJ     
        parts        NNS     NNS    
        for          IN      IN     
        Boeing       NNP     NNP    
        's           POS     POS    
        747          JJ      CD     
        jetliners    NN      NNS    
        .            .       .     

```

At the end of prediction, decoding() outputs

- evaluate postagging accuracy
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