# Ngram
In order to compile this program, the C++ compiler should support C++11.
### build
`$ make`
### dependency
[boost c++ library](http://www.boost.org/)
### usage
```
Allowed options:
  --help                produce help message
  --train               train ngram
  --test                test ngram
  --laplace             apply laplace smoothing
  -n arg (=1)           use  n-gram
  --file-train arg      train file path
  --file-test arg       test file path
```
### example
Train the unigram 
```
$ zcat foo.gz | ngram
```
Train the bigram 
```
$ zcat foo.gz | ngram 2 --laplace
```
Train the trigram 
```
$ ngram -n 3 --file-train='foo.txt'
```
Test the bigram 
```
$ zcat bar.gz | ngram 2 --test --laplace
```