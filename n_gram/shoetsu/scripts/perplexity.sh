METHOD="perplexity"
N=3
CORPUS="data/2013-01-01.cdr.gz"
NLINE=50


if [ $# -lt 1 ]; then
  echo "./perplexity (2|3) [N_GRAM_FILE] [NLINE] [CORPUS_FILE]"
  exit 1 
fi

N=$1

if [ $# -gt 1 ]; then
    FILE=$2
else
    if [ $N -eq 2 ]; then
	FILE="trained/bi_gram.dat"
    elif [ $N -eq 3 ]; then
	FILE="trained/tri_gram.dat"
    fi
fi

if [ $# -gt 2 ]; then
    NLINE=$3
fi
if [ $# -gt 3 ]; then
    CORPUS=$4
fi

echo "<Perplexity>"
echo "Test Corpus         : ${CORPUS}"
echo "Test Corpus lines   : ${NLINE}"
echo ""

gzip -dc $CORPUS | head -$NLINE | LC_ALL=C cut -f 1 | ./a.out $METHOD $N $FILE
