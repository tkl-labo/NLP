An implementation of CKY/Earley algorithms
===
+ to_cnf.py - convert CFG into Chomsky Normal Form
  - assume terminals are prefixed by '_'
  - assume no loopy unit productions (e.g., A -> B, B -> A)

            > python to_cnf.py < rules.txt > rules_cnf.txt

+ cky.py, earley.py - parse a sentence using CKY/Earley algorithm
  - interactive shell supports following three commands:
      - parse:  parse a sentence (earley.py outputs state transtion as well)
      - stat:   show #edges etc. after parsing
      - print:  print parse trees

                # python cky.py rules_cnf.txt
                > parse 'book the dinner flight'
                success
                > stat
                sentence: book the dinner flight
                length:   4
                # edges:  23
                # trees:  2
                > print
                tree #1
                (S
                  (Verb book)
                  (NP
                    (Det the)
                    (Nominal
                      (Nominal dinner)
                      (Noun flight))))
                
                tree #2
                (S
                  (Verb book)
                  (NP
                    (Det the)
                    (Nominal dinner))
                  (NP flight))
                
                >
                
                # python earley.py rules.txt
                Chart[0]:
                    S0   gamma -> . S                [ 0, 0]  Init
                    S1   S -> . NP VP                [ 0, 0]  Predictor
                    S2   S -> . Aux NP VP            [ 0, 0]  Predictor
                    S3   S -> . VP                   [ 0, 0]  Predictor
                    S4   NP -> . Pronoun             [ 0, 0]  Predictor
                    S5   NP -> . Proper-Noun         [ 0, 0]  Predictor
                    S6   NP -> . Det Nominal         [ 0, 0]  Predictor
                    S7   NP -> . Nominal             [ 0, 0]  Predictor
                    S8   VP -> . Verb                [ 0, 0]  Predictor
                    S9   VP -> . Verb NP             [ 0, 0]  Predictor
                    S10  VP -> . Verb NP PP          [ 0, 0]  Predictor
                    S11  VP -> . Verb PP             [ 0, 0]  Predictor
                    S12  VP -> . Verb NP NP          [ 0, 0]  Predictor
                    S13  VP -> . VP PP               [ 0, 0]  Predictor
                    S14  Nominal -> . Noun           [ 0, 0]  Predictor
                    S15  Nominal -> . Nominal Noun   [ 0, 0]  Predictor
                    S16  Nominal -> . Nominal PP     [ 0, 0]  Predictor
                Chart[1]:
                    S17  Verb -> book .              [ 0, 1]  Scanner
                    S18  Noun -> book .              [ 0, 1]  Scanner
                    S19  VP -> Verb .                [ 0, 1]  Completer
                    S20  VP -> Verb . NP             [ 0, 1]  Completer
                    S21  VP -> Verb . NP PP          [ 0, 1]  Completer
                    S22  VP -> Verb . PP             [ 0, 1]  Completer
                    S23  VP -> Verb . NP NP          [ 0, 1]  Completer
                    S24  Nominal -> Noun .           [ 0, 1]  Completer
                    S25  S -> VP .                   [ 0, 1]  Completer
                    S26  VP -> VP . PP               [ 0, 1]  Completer
                    S27  NP -> . Pronoun             [ 1, 1]  Predictor
                    S28  NP -> . Proper-Noun         [ 1, 1]  Predictor
                    S29  NP -> . Det Nominal         [ 1, 1]  Predictor
                    S30  NP -> . Nominal             [ 1, 1]  Predictor
                    S31  PP -> . Preposition NP      [ 1, 1]  Predictor
                    S32  NP -> Nominal .             [ 0, 1]  Completer
                    S33  Nominal -> Nominal . Noun   [ 0, 1]  Completer
                    S34  Nominal -> Nominal . PP     [ 0, 1]  Completer
                    S35  Nominal -> . Noun           [ 1, 1]  Predictor
                    S36  Nominal -> . Nominal Noun   [ 1, 1]  Predictor
                    S37  Nominal -> . Nominal PP     [ 1, 1]  Predictor
                    S38  S -> NP . VP                [ 0, 1]  Completer
                    S39  VP -> . Verb                [ 1, 1]  Predictor
                    S40  VP -> . Verb NP             [ 1, 1]  Predictor
                    S41  VP -> . Verb NP PP          [ 1, 1]  Predictor
                    S42  VP -> . Verb PP             [ 1, 1]  Predictor
                    S43  VP -> . Verb NP NP          [ 1, 1]  Predictor
                    S44  VP -> . VP PP               [ 1, 1]  Predictor
                Chart[2]:
                    S45  Det -> the .                [ 1, 2]  Scanner
                    S46  NP -> Det . Nominal         [ 1, 2]  Completer
                    S47  Nominal -> . Noun           [ 2, 2]  Predictor
                    S48  Nominal -> . Nominal Noun   [ 2, 2]  Predictor
                    S49  Nominal -> . Nominal PP     [ 2, 2]  Predictor
                Chart[3]:
                    S50  Noun -> dinner .            [ 2, 3]  Scanner
                    S51  Nominal -> Noun .           [ 2, 3]  Completer
                    S52  NP -> Det Nominal .         [ 1, 3]  Completer
                    S53  Nominal -> Nominal . Noun   [ 2, 3]  Completer
                    S54  Nominal -> Nominal . PP     [ 2, 3]  Completer
                    S55  VP -> Verb NP .             [ 0, 3]  Completer
                    S56  VP -> Verb NP . PP          [ 0, 3]  Completer
                    S57  VP -> Verb NP . NP          [ 0, 3]  Completer
                    S58  PP -> . Preposition NP      [ 3, 3]  Predictor
                    S59  S -> VP .                   [ 0, 3]  Completer
                    S60  VP -> VP . PP               [ 0, 3]  Completer
                    S61  NP -> . Pronoun             [ 3, 3]  Predictor
                    S62  NP -> . Proper-Noun         [ 3, 3]  Predictor
                    S63  NP -> . Det Nominal         [ 3, 3]  Predictor
                    S64  NP -> . Nominal             [ 3, 3]  Predictor
                    S65  Nominal -> . Noun           [ 3, 3]  Predictor
                    S66  Nominal -> . Nominal Noun   [ 3, 3]  Predictor
                    S67  Nominal -> . Nominal PP     [ 3, 3]  Predictor
                Chart[4]:
                    S68  Noun -> flight .            [ 3, 4]  Scanner
                    S69  Nominal -> Nominal Noun .   [ 2, 4]  Completer
                    S70  Nominal -> Noun .           [ 3, 4]  Completer
                    S71  NP -> Det Nominal .         [ 1, 4]  Completer
                    S72  Nominal -> Nominal . Noun   [ 2, 4]  Completer
                    S73  Nominal -> Nominal . PP     [ 2, 4]  Completer
                    S74  NP -> Nominal .             [ 3, 4]  Completer
                    S75  Nominal -> Nominal . Noun   [ 3, 4]  Completer
                    S76  Nominal -> Nominal . PP     [ 3, 4]  Completer
                    S77  VP -> Verb NP .             [ 0, 4]  Completer
                    S78  VP -> Verb NP . PP          [ 0, 4]  Completer
                    S79  VP -> Verb NP . NP          [ 0, 4]  Completer
                    S80  PP -> . Preposition NP      [ 4, 4]  Predictor
                    S81  VP -> Verb NP NP .          [ 0, 4]  Completer
                    S82  S -> VP .                   [ 0, 4]  Completer
                    S83  VP -> VP . PP               [ 0, 4]  Completer
                    S84  NP -> . Pronoun             [ 4, 4]  Predictor
                    S85  NP -> . Proper-Noun         [ 4, 4]  Predictor
                    S86  NP -> . Det Nominal         [ 4, 4]  Predictor
                    S87  NP -> . Nominal             [ 4, 4]  Predictor
                    S88  Nominal -> . Noun           [ 4, 4]  Predictor
                    S89  Nominal -> . Nominal Noun   [ 4, 4]  Predictor
                    S90  Nominal -> . Nominal PP     [ 4, 4]  Predictor
                success
                > stat
                sentence: book the dinner flight
                length:   4
                # states: 91
                # trees:  2
                > print
                parse tree #1
                (S 
                  (VP 
                    (Verb book)
                    (NP 
                      (Det the)
                      (Nominal 
                        (Nominal 
                          (Noun dinner))
                        (Noun flight)))))
                
                parse tree #2
                (S 
                  (VP 
                    (Verb book)
                    (NP 
                      (Det the)
                      (Nominal 
                        (Noun dinner)))
                    (NP 
                      (Nominal 
                        (Noun flight)))))




+ cky\_p.py, earley\_p.py - parse a sentence using probabilistic CKY/Earley algorithm

                # echo 'book the dinner flight' | python cky_p.py rules_cnf.txt
                prob: 1.62e-06
                (S 
                  (Verb book)
                  (NP 
                    (Det the)
                    (Nominal 
                      (Nominal dinner)
                      (Noun flight))))

                # echo 'book the dinner flight' | python earley_p.py rules.txt    
                prob: 1.62e-06
                (S 
                  (VP 
                    (Verb book)
                    (NP 
                      (Det the)
                      (Nominal 
                        (Nominal 
                          (Noun dinner))
                        (Noun flight)))))
