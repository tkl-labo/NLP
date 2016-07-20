#include "pos.h"
#include "cmdline.h"
#include <iostream>
#include <string>
#include <cstdio>
#include <unistd.h>

using namespace nlp;
using namespace std;

int main(int argc, char *argv[])
{
	cmdline::parser parser;

	parser.add<int>("ngram", 'n', "use ngram HMM", false, 2);
	parser.add<string>("train-file", 't', "specify training file", false, "");
	parser.add<string>("test-file", 'T', "specify test file", false, "");
	parser.add("train", '\0', "train the model");
	parser.add("test", '\0', "test the model");

	parser.parse_check(argc, argv);

	string train_file = parser.get<string>("train-file");
	string test_file = parser.get<string>("test-file");
	
	if (!parser.exist("train") && !parser.exist("test") && train_file=="" && test_file=="")
	{
		if (isatty(fileno(stdin)))
		{
			cerr << parser.usage();
			exit(0);
		}
	}

	int n = parser.get<int>("ngram");
	if (parser.exist("train") || !parser.exist("train") && !parser.exist("test"))
	{	
		POS pos(n);
		cout << "training..." << endl;
		pos.train(train_file);
	}

	if (parser.exist("test"))
	{
		POS pos(n);
		cout << "testing..." << endl;
		pos.test(test_file);
	}
}
