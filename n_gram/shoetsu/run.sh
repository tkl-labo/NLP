
corpus="data/2012-02-01.cdr.gz"
N=2
if [ $# -gt 0 ]; then
  corpus=$1
fi
if [ $# -gt 1 ]; then
  N=$2
fi
 
gzip -dc $corpus | cut -f 1 | ./a.out $N
