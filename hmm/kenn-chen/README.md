# Part-of-Speech

### build
`$ make`
### usage
```
usage: ./pos [options] ... 
options:
  -t, --train-file    specify training file (string [=])
  -T, --test-file     specify test file (string [=])
      --train         train the model
      --test          test the model
  -?, --help          print this message
```

### example
Train the data using bigram
```
$ zcat train.txt.gz | ./pos --train
```
Train the bigram using trigram
```
$ zcat foo.gz | ./pos --train
```
Test the model using bigram
```
$ ./pos --test --test-file='test.txt'
```
Test the model using trigram 
```
$ zcat test.txt.gz | ./pos --test
```