METHOD="create"
N=3
KEYWORDS=""

if [ $# -lt 1 ]; then
  echo "./create.sh (2|3) [loadfile]"
  exit 1
fi

N=$1




if [ $# -gt 1 ]; then
    FILE=$2
    if [ $# -gt 2 ]; then
	KEYWORDS=$3
    fi
else
    if [ $N -eq 2 ]; then
	FILE="trained/bi_gram.dat"
    elif [ $N -eq 3 ]; then
	FILE="trained/tri_gram.dat"
    fi
fi

./a.out $METHOD $N $FILE $KEYWORDS
    
