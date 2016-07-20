# POS Tagging
train.txt(形式は以下)から最尤推定でモデルパラメータを学習する。
test.txtにViterbiアルゴリズムを適用し評価する。

## Viterbiアルゴリズム実行方法
いきなり `./viterbi` を実行しても動かず、少しばかり必要なファイルがあるので、それらを先に生成する必要があります。
一気に作成してしまっているので、ファイル名は後に修正するかもしれません。
遷移確率のファイルが必要なので、

```
$ cat train.txt | ./txt2tagseq | ./tagseq2tagseq_count 1 > 1tagseq_count.txt
$ cat train.txt | ./txt2tagseq | ./tagseq2tagseq_count 2 > 2tagseq_count.txt
```

尤度のファイルも必要なので、

```
$ cat train.txt | ./txt2tagword_count > obs_likeli.txt
```

いよいよviterbiを実行。上記で出力した3ファイルを使用して、

```
$ cat test.txt | ./viterbi 2tagseq_count.txt 1tagseq_count.txt obs_likeli.txt
```

## ファイル群の構成
* Makefile:以下のコマンドで実行ファイル群を生成できる。詳細は中身を参照。

```
$ make
```

* common.h: 共通で使うであろうライブラリや変数など

### 遷移確率に関わる2ファイル
* txt2tagseq.cpp: 解析済みファイル(train.txt, test.txt)からタグの並びを抽出するスクリプト。一文の単語のタグを一行に出力する。以下は実行例。

```
$ cat train.txt | ./txt2tagseq  | head -3
NN IN DT ... POS JJ NNS .
NNP IN DT ... DT JJ NN .
CC NNS VBP ... NN JJ NNP .
```

* tagseq2tagseq_count.cpp: 一文のタグの並びから指定したn個のタグ順を取得するスクリプト。txt2tagseqの出力を受け取って用いる。以下は実行例

```
$ cat train.txt | ./txt2tagseq | ./tagseq2tagseq_count 1 | head -5
36 #
1750 $
1493 ''
274 (
281 )
$ cat train.txt | ./txt2tagseq | ./tagseq2tagseq_count 2 | head -5
36 # CD
1724 $ CD
7 $ JJ
16 $ NN
1 $ NNP
```
### 尤度に関わる1ファイル
* txt2tagword_count.cpp: 解析済みファイルからタグと単語の組をカウントするスクリプト。

```
$ cat train.txt | ./txt2tagword_count | tail -10
77 WRB where
3 WRB wherever
20 WRB why
28 `` `
1503 `` ``
```

### Viterbiアルゴリズムに関わる1ファイル
* viterbi.cpp: viterbiを計算するスクリプト。Transition Probability, Observation LikelihoodをLaplace Smoothingする。2つの仮定を置いている。
	* タグは直前のタグにのみ依存する
	* 単語の出現する確率はそのタグにのみ依存する

```
$ cat test.txt | ./viterbi n_tag_trans_count.txt n-1_tag_trans_count.txt obs_likli.txt
NN IN DT ... JJ NNS .
NNP IN DT ... DT JJ NN .
...
DT NN , ... NNP NNP NNP .
total tags: 47377, correct tags: 40724
precision: 0.859573
```

### Forwardアルゴリズムに関わるファイル
* forward.cpp: forwardアルゴリズムを実行するスクリプト。各行に各文の尤度を出力する。仮定や引数などはviterbiと同じ。
```
$ cat test.txt | ./forward n_tag_trans_count.txt n-1_tag_trans_count.txt obs_likli.txt | head -3
5.701039134822774e-84
1.6877287082313187e-48
1.020969742467284e-91
```

## 解析済みファイル(train.txt, test.txt)の形式
```
He PRP B-NP
is VBZ B-VP
(He is から始まる一文の他の単語のタグ等の情報)
deficits NNS I-NP
. . O
(改行で文の区切り)
```