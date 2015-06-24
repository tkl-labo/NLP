METHOD="perplexity"
N=3

if [ $# -lt 1 ]; then
  echo "./perplexity (2|3) [filename]"
  exit 1 
fi

N=$1

if [ $# -gt 1 ]; then
    FILE=$2
else
    if [ $N -eq 2 ]; then
	FILE="data/bi_gram.dat"
    elif [ $N -eq 3 ]; then
	FILE="data/tri_gram.dat"
    fi
fi
./a.out $METHOD $N $FILE
