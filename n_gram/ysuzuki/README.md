学習に時間がかかってしまい，ngramの学習までしかできませんでした

#作ったコード

##ngramCrawler.py

研究室の解析済みツイートコーパスの情報を，扱いやすい形で取り出してくれるwrapper(?)

###関数，class

####def dataFetcher(date)

日付を引数に取ると，ツイートコーパスのsentenceを順に返してくれるgenerator

####class ngramCorpus

ツイートコーパスのsentenceを引数に取ると，情報を整理してくれるclass

ngramMakerメソッドでnを指定すると，n個のwordのtupleを返してくれる

####def counter(date,n)

dateとnを指定すると，その日付のツイートコーパスに対して，1~n gramまで出現回数をカウントし，tsv fileに書き込む

###appendix

研究室のツイートデータに対しても同じようなものを作ってよく使っている(/home/ysuzuki/local/arilib/python2.7/tweet.py)

usage(tweet.py)

```
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