NLP - n_gram
======================
NLP meetingでのhmmの実装プロジェクト
 
使い方
------
### `hmm.py`
    テストデータからを数え上げるプログラム。

    python3.x bigram.py hogehoge.cdr.gz  

### `ngram.py`
    ngramの度数を数え、perplexityを出力するプログラム

    python3.x ngram.py [1~10000000] [1~10000000]
 
 bigram.pyの詳細
----------------
bigramを数え上げるプログラム。二次元辞書を用いた実装になっているが、タプルをキーにすればいいだけという頭の悪い実装。

 ngram.pyの実装
----------------
ngramの度数を数え、perplexityを出力するプログラム。突貫的に作ったので、関数分けができてない。汚い。
一つ目の引数に、学習するサイズ( ~ynaga/ngrams/data/2012-01-01.cdr.xxxxxx.gz)
二つ目の引数に、テストのサイズ( ~ynaga/ngrams/data/2013-01-02.cdr.xxxxxx.gz)
を指定することで、perplexityを算出し、出力する。

関連情報
--------
### 二次元辞書
1. [リンク1](http://materia.jp/blog/20121119.html "Pythonで多次元の辞書を扱う [Python]")
 
ライセンス
----------
Copyright &copy; 2014 @higitune
 

