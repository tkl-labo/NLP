#Hiragana-SpellChecker

## edit_distance.py

編集距離を算出(unicode対応)

```
$ python edit_distance.py <word1> <word2>
```

```
$ python edit_distance.py まじか まじで
2
```

## hiragana_distance.py
ひらがな/カタカナをローマ字の入力に直して編集距離を算出

```
$ python hiragana_distance.py <word1> <word2>
```

```
$ python hiragana_distance.py けいたいでんわ けいたいｄんわ
1

$ python hiragana_distance.py ｍｊｋｙ まじかよ
4
```

## spellchecker.py
**ひらがな/カタカナ のスペルミスについて，正しいスペルの候補を確率付きで複数提示する**

###Usage (ただし，辞書を読み込むのに20秒位かかる…)

```
$ python spellchecker.py <word>

$ python spellchecker.py こんにｃｈわ
romaji:kon'nichwa
 1 : こんにちわ(kon'nichiwa)        1.18839924876e-07
 2 : こんにちわ(kon'nichiwa)        3.96133082921e-08
 3 : こんにちゎ(kon'nichixwa)       6.09435512185e-09

#引数を指定しないとtest caseを表示
$ python spellchecker.py
(testcase result)

```

###テスト例(ありそうなスペルミス)

```
################見方#################
<スペルミス>, correct: <正しいスペル>
romaji:<スペルミスのローマ字>
 1 : <第1候補>(ローマ字)	確率
 2 : <第2候補>(ローマ字)	確率
…
######################################

いｔｓも, correct:いつも
romaji:itsmo
 1 : いつも (itsumo)             7.29401992607e-05
 2 : いつも (itsumo)             2.43133997536e-05
 3 : いても (itemo)              8.28356516415e-06

ぽけん, correct:ポケモン
romaji:poken
 1 : ぽけもん(pokemon)            6.80667483125e-06

ゆうおｋりん, correct:ゆうこりん
romaji:yuuokrin
 1 : ゆうこりん(yuukorin)           8.2273794145e-08
 2 : ゆうこりん(yuukorin)           2.74245980484e-08

りらｋま, correct:りらっくま
romaji:rirakma
 1 : りらっくま(rirakkuma)          2.74245980484e-08

なんｄあｙろ, correct:なんでやろ
romaji:nandayro
 1 : なんでやろ(nandeyaro)          1.85877831217e-07
 2 : なんだよろ(nandayoro)          2.74245980484e-08
 3 : なんだよお(nandayoo)           2.43774204874e-08

でｈそ, correct:でしょ
romaji:dehso
 1 : でしょ (desho)              9.3678279654e-05
 2 : でしょう(deshou)             2.30464080251e-05
 3 : んでしょ(ndesho)             7.85335959375e-06


##想定したのとは違う候補が出た例##

ほんたの, correct:ほんとなの
romaji:hontano
 1 : ほんとあの(hontoano)           2.10255251704e-07
 2 : ほんとあの(hontoano)           7.00850839015e-08
 3 : ほんとなの(hontonano)          5.18020185355e-08

まじきょ, correct:まじかよ
romaji:majikyo
 1 : まじかよお(majikayoo)          2.43774204874e-08
 2 : いまじかよ(imajikayo)          2.13302429265e-08
 3 : まじかよう(majikayou)          1.52358878047e-08


##辞書が貧弱なので，NO RESULTが結構出る##

つらぴょ, correct:つらぽよ
romaji:tsurapyo
NO RESULT

うそｄｓ, correct:うそだ
romaji:usods
NO RESULT

けいたいｄんわ, correct:けいたいでんわ
romaji:keitaidnwa
NO RESULT

```

参考:How to Write a Spelling Corrector
http://norvig.com/spell-correct.html

##簡単な解説
c = correct word
w = word spell

P(c|w) = P(w|c)*P(c)

1.P(c): language model
       それぞれの語が出現する確率
       今回はひらがなorカタカナの1~7gramの出現確率を
       研究室のtwitter data(2013/07/17)から学習した
       学習した結果のリストは
       ngram_data/kana_freq.tsv(10万語) にある

2.P(w|c): error model
	  スペルミスをしない確率>>編集距離1のスペルミスをする確率>>編集距離2のスペルミスをする確率
	  であると過程

	  編集距離に応じて，適当に下記のように設定した
	  edit_distance=1 :	0.2
	  edit_distance=2 :	0.05

