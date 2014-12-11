CKY Parser
===
+ cky.py - CKYアルゴリズムを用いた構文解析器
- 文法/語彙ファイル: `./data/rules.cnf`
- 注意: 上記の文法ファイルは文脈自由文法からチョムスキー標準形に変換してあります。
- 使い方:
```
    python cky.py book me a flight to Houston
     S
    - Verb ( book )
    - NP ( me )
    - NP
    -- Det ( a )
    -- Nominal
    --- Nominal ( flight )
    --- PP
    ---- Preposition ( to )
    ---- NP ( Houston )
     S
    - VP
    -- Verb ( book )
    -- NP ( me )
    -- NP
    --- Det ( a )
    --- Nominal ( flight )
    - PP
    -- Preposition ( to )
    -- NP ( Houston )
```

+ pcky.py - 上記の確率版
- 文法/語彙ファイル: `./data/rules_p.cnf`
- 注意: 上記の文法ファイルは文脈自由文法からチョムスキー標準形に変換してあります。
- 使い方:
```
    python pcky.py book me a flight to Houston
     S [ 1.43521875e-09 ]
    - Verb ( book )
    - NP ( me )
    - NP
    -- Det ( a )
    -- Nominal
    --- Nominal ( flight )
    --- PP
    ---- Preposition ( to )
    ---- NP ( Houston )
     S [ 4.30565625e-09 ]
    - VP
    -- Verb ( book )
    -- NP ( me )
    -- NP
    --- Det ( a )
    --- Nominal ( flight )
    - PP
    -- Preposition ( to )
    -- NP ( Houston )
```
