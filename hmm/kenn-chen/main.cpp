#include "pos.h"
#include "cmdline.h"
#include <iostream>
#include <string>

using namespace nlp;
using namespace std;

int main(int argc, char *argv[])
{
	cmdline::parser parser;

	parser.add<int>("ngram", 'n', "use ngram HMM", false, 2);
	parser.add<string>("train-file", 't', "specify training file", false, "");
	parser.add<string>("test-file", 'T', "specify test file", false, "");
	parser.add("train", '\0', "training the data");
	parser.add("test", '\0', "test the model");

	parser.parse_check(argc, argv);

	string train_file = parser.get<string>("train-file");
	string test_file = parser.get<string>("test-file");
	
	if (!parser.exist("train") && !parser.exist("test") && train_file=="" && test_file=="")
	{
		cerr << parser.usage();
		exit(0);
	}
	int n = parser.get<int>("ngram");
	if (parser.exist("train") || !parser.exist("train") && !parser.exist("test"))
	{	
		POS pos(n);
		pos.train(train_file);
	}

	if (parser.exist("test"))
	{
		POS pos(n);
		pos.test(test_file);
	}
}
