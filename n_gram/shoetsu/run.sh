METHOD="load"
NLINE=500000
N=3
CORPUS="data/2012-01-01.cdr.gz"

if [ $# -gt 0 ]; then
  METHOD=$1
fi
if [ $# -gt 1 ]; then
  N=$2
fi

if [ $METHOD = "learn" ]; then
   if [ $# -gt 2 ]; then
       NLINE=$3
   fi
   if [ $# -gt 3 ]; then
       CORPUS=$4
   fi
   gzip -dc $CORPUS | head -$NLINE | LC_ALL=C cut -f 1 | ./a.out $METHOD $N
else
   ./a.out $METHOD $N
fi

#gzip -dc $CORPUS |  LC_ALL=C cut -f 1 | ./a.out 2
