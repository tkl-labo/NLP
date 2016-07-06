## 構成
- cdr2sentence.cpp : 形態素解析結果のcdrファイルを、各単語をスペースで、各文を改行で区切る文に変換するスクリプト。出力は標準出力
- sentence2ngram.cpp : 単語間がスペースで、文が改行で区切られた文からngramをカウントするスクリプト。出力は標準出力。
- perplexity.cpp : perplexityを計算するスクリプト。出力は標準出力。
- common.h, common.cpp : 各ファイルで使う関数や定数の定義
- utility.h, utility.cpp : デバッグ用（vectorやmapの中身確認）

## 実行方法
```
$ make
$ ./cdr2sentence cdr_file
$ ./sentence2ngram sentence_file N
$ ./perplexity N ngram_file n-1gram_file test_file   # 引数多すぎるので修正したい
```