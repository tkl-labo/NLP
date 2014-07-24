学習に時間がかかってしまい，ngramの学習までしかできませんでした

##作ったコード

###ngramCrawler.py
研究室の解析済みツイートコーパスの情報を，扱いやすい形で取り出してくれる

**def dataFetcher(date)**
日付を引数に取ると，ツイートコーパスのsentenceを順に返してくれるgenerator

**class ngramCorpus**
ツイートコーパスのsentenceを引数に取ると，情報を整理してくれるclass
ngramMakerメソッドでnを指定すると，n個のwordのtupleを返してくれる

**def counter(date,n)**
dateとnを指定すると，その日付のツイートコーパスに対して，1~n gramまで出現回数をカウントし，tsv fileに書き込む

###makefile.py
同じプログラムに異なる日付の引数を与えて，並列化できるようなMakefileを作ってくれる