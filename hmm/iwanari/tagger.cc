#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cmath>
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
		
		// count the frequency of POS
		m_posFreqs[cur_pos]++;
		
		// the end of sentence
		if (rows.size() == 1) {
			// added end symbol
			m_succFreqs[cur_pos][END_SYMBOL]++;
			// init
			cur_pos = START_SYMBOL;
		}
		else {
			m_succFreqs[cur_pos][rows[1]]++;
			cur_pos = rows[1];
			
			// count the frequency of (word, pos) pair
			m_wordPosFreqs[rows[0]][rows[1]]++;
			m_wordFreqs[rows[0]]++;
		}
	}

	input_file.close();
	std::cout << "trained!" << std::endl;
}

void Tagger::forwardTest(std::ifstream &input_file)
{
	std::string line;
	std::string cur_pos = START_SYMBOL;
	double logLikelihood = 0.0;
	
	while(std::getline(input_file, line)) {
		// rows[0]: word, rows[1]: POS
		std::vector<std::string> rows 
			= Util::splitString(line, DELIME_IN_CORPUS);
		
		// the end of sentence
		if (rows.size() == 1) {
			// calculate likelihood
			std::cout << std::endl;
			std::cout << "Likelihood: " << std::exp(logLikelihood);
			
			// init
			std::cout << std::endl << std::endl;
			cur_pos = START_SYMBOL;
			logLikelihood = 0.0;
		}
		else {
			// calculate likelihood in log
			// TODO: zero check
			logLikelihood += std::log(m_succFreqs[cur_pos][rows[1]] / (double) m_posFreqs[cur_pos]);
			logLikelihood += std::log(m_wordPosFreqs[rows[0]][rows[1]] / (double) m_wordFreqs[rows[0]]);
			std::cout << rows[0] << " ";
			cur_pos = rows[1];
		}
	}
}

void Tagger::viterbiTest(std::ifstream &stream)
{
}


void Tagger::test(const std::string &testing)
{
	std::cout << "testing..." << std::endl;
	std::ifstream input_file(testing);
	if (!input_file) {
		std::cerr << "ERROR: No such file (" 
			<< testing << ")" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	
	switch (m_mode) {
		case 0:
			forwardTest(input_file);
			break;
		case 1:
			viterbiTest(input_file);
			break;
		default:
			forwardTest(input_file);
			break;
	}
	
	input_file.close();
	std::cout << "tested!" << std::endl;
}



// ===== for debug =====
void Tagger::showSuccProbs()
{
	for (auto f : m_succFreqs) {
		long total = m_posFreqs[f.first];
		for (auto s : f.second) {
			std::cout << "Prob("
				<< s.first << " | "
				<< f.first << ") = " 
				<< s.second / (double) m_posFreqs[f.first] 
				<< std::endl;
		}
		std::cout << "=======" << std::endl << std::endl;
	}
}

void Tagger::showWordPosProbs()
{
	for (auto f : m_wordPosFreqs) {
		for (auto s : f.second) {
			std::cout << "Prob(" 
				<< s.first << " | "
				<< f.first << ") = " 
				<< s.second / (double) m_wordFreqs[f.first] 
				<< std::endl;
		}
		std::cout << "=======" << std::endl << std::endl;
	}
}

void Tagger::showAllProbs()
{
	showSuccProbs();
	
	std::cout << "************" << std::endl << std::endl;
	
	showWordPosProbs();
}
// ===== /for debug =====

