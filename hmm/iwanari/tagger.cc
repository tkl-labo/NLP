#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include "nlp.h"
#include "util.h"
#include "tagger.h"
using namespace nlp;

void Tagger::init()
{
	m_succFreqs.clear();
}

void Tagger::train(const std::string &training)
{
	init();
	
	std::cout << "training..." << std::endl;
	std::ifstream input_file(training);
	if (!input_file) {
		std::cerr << "ERROR: No such file (" 
			<< training << ")" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	std::string line;
	std::string cur_pos = START_SYMBOL;

	while(std::getline(input_file, line)) {
		// rows[0]: word, rows[1]: POS
		std::vector<std::string> rows 
			= Util::splitString(line, DELIME_IN_CORPUS);
		
		m_freqs[cur_pos]++;
		
		// end of sentence
		if (rows.size() == 1) {
			m_succFreqs[cur_pos][END_SYMBOL]++;
			cur_pos = START_SYMBOL;
		}
		else {
			m_succFreqs[cur_pos][rows[1]]++;
			cur_pos = rows[1];
		}
	}

	input_file.close();
	std::cout << "trained!" << std::endl;
}

void Tagger::test(const std::string &testing)
{
}

void Tagger::showAllProbabilities()
{
	for (auto f : m_succFreqs) {
		long total = m_freqs[f.first];
		for (auto s : f.second) {
			std::cout << "Prob(" << f.first << ", "
				<< s.first << ") = " << 
				s.second / (double) total << std::endl;
		}
		std::cout << "=======" << std::endl << std::endl;
	}
}
