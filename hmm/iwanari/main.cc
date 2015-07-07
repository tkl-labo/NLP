#include <iostream>
#include <wchar.h>
#include <memory>
#include "tagger.h"
#include "tri_tagger.h"
#include "stopwatch.h"
using namespace nlp;

int main(int argc, char** argv)
{
	if (argc < 4) {
		std::cout << "Usage: " << argv[0] 
			<< " training_data testing_data (Forward:0 | Viterbi:1) [debug = 0]" 
			<< std::endl;
		return 0;
	}

	setlocale(LC_ALL, "ja_JP.UTF-8");

	util::Stopwatch<std::chrono::milliseconds> sw;
	TriTagger tagger(std::atoi(argv[3]), 
				(argc > 4) && std::atoi(argv[4]) != 0);
	tagger.train(argv[1]);
	sw.stop();
	std::cout << "Elapsed (training): " 
		<< sw.showElapsedTime().c_str() << std::endl;
	std::cout << "-------" << std::endl;

	// tagger.showSuccProbs();
	// tagger.showWordPosProbs();
	// tagger.showAllProbs();
	
	sw.start();
	tagger.test(argv[2]);
	sw.stop();
	std::cout << "Elapsed (test): " 
		<< sw.showElapsedTime().c_str() << std::endl;
	std::cout << "-------" << std::endl;

	return 0;
}

