METHOD="learn"
NLINE=500000
N=3
CORPUS="data/2012-01-01.cdr.gz"
#CORPUS="/net/157.82.157.42/vol/volExpr2/kaji/tweet/analyzed/2012/01/01.cdr.gz"

if [ $# -lt 1 ]; then
  echo "./learn.sh (2|3) [CORPUS_FILE.gz]"
  exit 1 
fi

N=$1

if [ $# -gt 1 ]; then
    NLINE=$2
fi
if [ $# -gt 2 ]; then
    CORPUS=$3
fi
gzip -dc $CORPUS | head -$NLINE | LC_ALL=C cut -f 1 | ./a.out $METHOD $N
