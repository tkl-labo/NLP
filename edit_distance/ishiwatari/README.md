## Usage

### edit_distance.py

```
$ python3 edit_distance.py うほ うほうほ

うほ うほうほ
[[ 0.  1.  2.  3.  4.]
 [ 1.  0.  1.  2.  3.]
 [ 2.  1.  0.  1.  2.]]
MIN_DISTANCE:  2.0
```


### spell_correction.py

```
$ python3 spell_correction.py hoge

Do you mean...
hage  ( 2 )
hoagie  ( 2 )
hodge  ( 1 )
hoe  ( 1 )
hoes  ( 2 )
hog  ( 1 )
hogged  ( 2 )
hole  ( 2 )
homage  ( 2 )
home  ( 2 )
hone  ( 2 )
hope  ( 2 )
hose  ( 2 )
howe  ( 2 )
huge  ( 2 )
loge  ( 2 )
ogle  ( 2 )
ogre  ( 2 )
shoe  ( 2 )
correction candidates:  ['hog', 'hoe', 'hodge', 'hage', 'hoagie', 'hoes', 'hogged', 'hole', 'homage', 'home', 'hone', 'hope', 'hose', 'howe', 'huge', 'loge', 'ogle', 'ogre', 'shoe']
```
