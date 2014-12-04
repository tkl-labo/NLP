An implementation of CKY/Earley algorithms
===
+ to_cnf.py - convert CFG into Chomsky Normal Form
  - assume terminals are prefixed by '_'
  - assume no loopy unit productions (e.g., A -> B, B -> A)

            > python to_cnf.py < rules.txt > rules_cnf.txt

+ cky.py, earley.py - parse a sentence using CKY/Earley algorithm
  - interactive shell supports following three commands
    - parse:  parse a sentence (earley.py outputs state transtion as well)
    - stat:   show #edges etc. after parsing
    - print:  print parse trees

            > parse 'book the dinner flight'
            success
            > stat
            sentence: book the dinner flight
            length:   4
            # edges:  25
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
            
