#include "ngram.h"

using namespace std;
using namespace nlp;

int main()
{
	Ngram bigram(3);
	bigram.train("../files/01.cdr.mini");
	bigram.test("../files/01.cdr.mini");
}