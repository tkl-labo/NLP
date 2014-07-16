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
*ひらがな/カタカナ のスペルミスについて，正しいスペルを確率付きで複数候補を提示する*

###Usage (ただし，辞書を読み込むのに数十秒かかります…)

```
$ python spellchecker.py <word>

$ python spellchecker.py こんにｃｈわ
romaji:kon'nichwa
 0 : こんにちわ(kon'nichiwa)        1.18839924876e-07
 1 : こんにちわ(kon'nichiwa)        3.96133082921e-08
 2 : こんにちゎ(kon'nichixwa)       6.09435512185e-09

#引数を指定しないとtest caseを表示
$ python spellchecker.py
(testcase result)

```

###テスト例

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
 0 : いつも (itsumo)             7.29401992607e-05
 1 : いつも (itsumo)             2.43133997536e-05
 2 : いても (itemo)              8.28356516415e-06

ぽけん, correct:ポケモン
romaji:poken
 0 : ぽけもん(pokemon)            6.80667483125e-06

ゆうおｋりん, correct:ゆうこりん
romaji:yuuokrin
 0 : ゆうこりん(yuukorin)           8.2273794145e-08
 1 : ゆうこりん(yuukorin)           2.74245980484e-08

りらｋま, correct:りらっくま
romaji:rirakma
 0 : りらっくま(rirakkuma)          2.74245980484e-08

なんｄあｙろ, correct:なんでやろ
romaji:nandayro
 0 : なんでやろ(nandeyaro)          1.85877831217e-07
 1 : なんだよろ(nandayoro)          2.74245980484e-08
 2 : なんだよお(nandayoo)           2.43774204874e-08

でｈそ, correct:でしょ
romaji:dehso
 0 : でしょ (desho)              9.3678279654e-05
 1 : でしょう(deshou)             2.30464080251e-05
 2 : んでしょ(ndesho)             7.85335959375e-06


##想定したのとは違う候補が出た例##

ほんたの, correct:ほんとなの
romaji:hontano
 0 : ほんとあの(hontoano)           2.10255251704e-07
 1 : ほんとあの(hontoano)           7.00850839015e-08
 2 : ほんとなの(hontonano)          5.18020185355e-08

まじきょ, correct:まじかよ
romaji:majikyo
 0 : まじかよお(majikayoo)          2.43774204874e-08
 1 : いまじかよ(imajikayo)          2.13302429265e-08
 2 : まじかよう(majikayou)          1.52358878047e-08


##適当に打って文字列を作製した例##

いてもあｗ, correct:?
romaji:itemoaw
 0 : いてもらわ(itemorawa)          5.78963736575e-08
 1 : いてもあん(itemoan)            3.65661307311e-08
 2 : いてもかわ(itemokawa)          1.82830653656e-08


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

参考:http://norvig.com/spell-correct.html