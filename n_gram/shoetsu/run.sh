
CORPUS="data/2012-02-01.cdr.gz"
NLINE=500000
if [ $# -gt 0 ]; then
  NLINE=$1
fi
if [ $# -gt 1 ]; then
  CORPUS=$2
fi
 
gzip -dc $CORPUS | head -$NLINE | LC_ALL=C cut -f 1 | ./a.out 2
