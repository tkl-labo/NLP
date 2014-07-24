学習に時間がかかってしまい，ngramの学習までしかできませんでした

#作ったコード

##ngramCrawler.py

研究室の解析済みツイートコーパスの情報を，扱いやすい形で取り出してくれるwrapper(?)

###関数，class

####def dataFetcher(date)

日付を引数に取ると，ツイートコーパスのsentenceを順に返してくれるgenerator

####class ngramCorpus

ツイートコーパスのsentenceを引数に取ると，情報を整理してくれるclass

ngramMakerメソッドでnを指定すると，ngramのtupleを返してくれる

usage:

```
-bash-3.2$ python
Python 2.7.6 (default, Mar 14 2014, 18:55:50) 
[GCC 4.1.2 20080704 (Red Hat 4.1.2-54)] on linux2
Type "help", "copyright", "credits" or "license" for more information.
>>> import ngramCrawler as n

>>> for sentence in n.dataFetcher("20121210"):
...     s = n.ngramCorpus(sentence)                                                                                                    
...     print s.surface
...     print s.ngramMaker(3)
... 
OPEN FILE: ../ngramCorpus/2012/12/10.cdr.gz
['\xe3\x83\x9e\xe3\x83\xaa\xe3\x83\x95\xe3\x82\xa1\xe3\x83\x8a\xe3\x83\x9c\xe3\x83\xbc\xe3\x82\xa4\xe3\x82\xba', '\xe3\x80\x81', '\xe3\x82\xb9\xe3\x83\x91\xe3\x83\xab\xe3\x82\xbf', '\xe7\x89\xb9\xe8\xa8\x93', '\xe3\x81\xaa\xe3\x81\x86']
[('<s>', '<s>', '\xe3\x83\x9e\xe3\x83\xaa\xe3\x83\x95\xe3\x82\xa1\xe3\x83\x8a\xe3\x83\x9c\xe3\x83\xbc\xe3\x82\xa4\xe3\x82\xba'), ('<s>', 
'\xe3\x83\x9e\xe3\x83\xaa\xe3\x83\x95\xe3\x82\xa1\xe3\x83\x8a\xe3\x83\x9c\xe3\x83\xbc\xe3\x82\xa4\xe3\x82\xba', '\xe3\x80\x81'), ('\xe3\x
83\x9e\xe3\x83\xaa\xe3\x83\x95\xe3\x82\xa1\xe3\x83\x8a\xe3\x83\x9c\xe3\x83\xbc\xe3\x82\xa4\xe3\x82\xba', '\xe3\x80\x81', '\xe3\x82\xb9\xe
3\x83\x91\xe3\x83\xab\xe3\x82\xbf'), ('\xe3\x80\x81', '\xe3\x82\xb9\xe3\x83\x91\xe3\x83\xab\xe3\x82\xbf', '\xe7\x89\xb9\xe8\xa8\x93'), ('
\xe3\x82\xb9\xe3\x83\x91\xe3\x83\xab\xe3\x82\xbf', '\xe7\x89\xb9\xe8\xa8\x93', '\xe3\x81\xaa\xe3\x81\x86'), ('\xe7\x89\xb9\xe8\xa8\x93', '\xe3\x81\xaa\xe3\x81\x86', '</s>'), ('\xe3\x81\xaa\xe3\x81\x86', '</s>', '</s>')]
…
```


####def counter(date,n)

dateとnを指定すると，その日付のツイートコーパスに対して，1~n gramまで出現回数をカウントし，tsv fileに書き込む

###appendix

研究室のツイートデータに対しても同じようなものを作ってよく使っている(/home/ysuzuki/local/arilib/python2.7/tweet.py)

usage(tweet.py)

```bash:
-bash-3.2$ python
Python 2.7.6 (default, Mar 14 2014, 18:55:50) 
[GCC 4.1.2 20080704 (Red Hat 4.1.2-54)] on linux2
Type "help", "copyright", "credits" or "license" for more information.

>>> import tweet
>>> rawtweets = tweet.raw_tweet("20140717")                         #日付指定

>>> for raw in rawtweets:
     t = tweet.tweet(raw)					#rawlineをtweet classに入れ，扱いやすい形にする
     print t.userid,t.screen_name,t.geo_lat,t.geo_long           

 
d 1/2 84.8MB /home/ysuzuki/yotsuya/twi.data/20140717.s.294.gz 3.7min
103215919 Bu_OMin - -
1142798748 JamesPakkanit - -
115704513 ca_wase - -
115903534 DocArnica - -
129128059 Dethtooldo - -
…
>>> 

```

##makefile.py

同じプログラムに異なる日付の引数を与えて，並列化できるようなMakefileを作ってくれる