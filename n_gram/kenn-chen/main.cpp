#include "ngram.h"
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdio>
#include <unistd.h>

namespace po = boost::program_options;
using namespace std;
using namespace nlp;

int main(int argc, char *argv[])
{
	int n = 1;
    string train_file = "";
    string test_file  = "";
	Smoothing smoothing = none;

	cout.precision(6);

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("train", "train ngram")
		("test", "test ngram")
		("laplace", "apply laplace smoothing")
		(",n", po::value<int>(&n)->default_value(1), "use  n-gram")
		("file-train", po::value<string>(&train_file)->default_value(""), "train file path")
		("file-test", po::value<string>(&test_file)->default_value(""), "test file path")
	;
	po::positional_options_description p;
	p.add("-n", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	po::notify(vm);    

	if (vm.count("help")) 
	{
    		cout << desc << endl;
    		return 1;
	}
	
	if (vm.count("train") && vm.count("test") && train_file=="" && test_file=="")
	{
		cout << desc << endl;
		return 1;
	}

	if (vm.count("laplace"))
		smoothing = laplace;

	if (vm.count("train") || !vm.count("train")&&!vm.count("test"))
	{	if (train_file == "" && isatty(fileno(stdin)))
		{
			cout << "No train file specified!" << endl;
			cout << desc << endl;
			return 1;
		}
		else
		{
			Ngram ngram(n, smoothing);
			ngram.train(train_file);
		}
	}	
	if (vm.count("test"))
	{	if (test_file == "" && isatty(fileno(stdin)))
		{
			cout << "No test file specified!" << endl;
			cout << desc << endl;
			return 1;
		}
		else
		{
			Ngram ngram(n, smoothing);
			ngram.test(test_file);
		}
	}
}
