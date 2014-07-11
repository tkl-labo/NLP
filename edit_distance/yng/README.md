minimum edit distance and its applications
===
+ edist.py - a plain implementation of minimum edit distance
  - compute minimum edit distance between given two strings
  - multi-byte string support
  - interactive shell

             > python edist.py 
             > invention execution
             dist ('invention', 'execution') = 8
+ align.py - a simple interactive aligner
  - compute an optimal alignment between given two strings
  - efficient generator-based alignment enumeration
  - multi-byte string support
  - interactive shell

            > 僕は友達が少ない はがない
            dist ('僕は友達が少ない', 'はがない') = 4
            alignment:
                action:	i s i i s i s s 
                source:	僕は友達が少ない
                target:	  は    が  ない
            -- stop enumeration? [y]: 
+ spell.cc - a simple interactive spell checker
  - enumerate similar strings in a given dictionary for a given string
  - scalable to a large-scale dictionary with millions of strings
  - interactive shell

            > g++ -O2 -g spell.cc -o spell
            > ./spell
            > percepiton
                    perception: 2
            -- stop enumeration? [y]:
                    percept: 3
            -- stop enumeration? [y]: y
