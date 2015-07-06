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

std::vector<std::pair<std::string, std::string>> nextSenetence(std::ifstream &stream)
{
	std::string line;
	// sentence contains (word, ans) pair
	std::vector<std::pair<std::string, std::string>> sentence;
	while(std::getline(stream, line)) {
		// rows[0]: word, rows[1]: POS
		std::vector<std::string> rows 
			= Util::splitString(line, DELIME_IN_CORPUS);
		
		// the end of sentence
		if (rows.size() == 1)
			return sentence;
		
		sentence.emplace_back(rows[0], rows[1]);
	}
	return sentence;
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
			// TODO: is this necessary?
			logLikelihood += std::log(m_succFreqs[cur_pos][END_SYMBOL] / (double) m_posFreqs[cur_pos]);
			
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

void Tagger::viterbiTest(std::ifstream &input_file)
{
	std::cout << "START: " << m_posFreqs[START_SYMBOL] << std::endl;
	// get sentence one by one
	// sentence contains (word, ans) pair
	std::vector<std::pair<std::string, std::string>> sentence;
	
	while((sentence = nextSenetence(input_file)).size() != 0) {

		const int sequenceLength = sentence.size() + 2;

		// current POS -> (prob, previous POS)
		std::vector<ScoreList> scores(sequenceLength);
		
		// start state
		// scores[0].emplace(START_SYMBOL, std::make_pair(1.0, nullptr));
		scores[0].emplace(START_SYMBOL, std::make_pair(1.0, ""));
		
		// end state
		sentence.emplace_back(END_SYMBOL, END_SYMBOL);
		for (int i = 1; i < sequenceLength; i++) {
			std::string word = sentence.at(i - 1).first;

			for (auto prev : scores[i - 1]) {
				// prev.first: POS, prev.second: <prob, previous POS>
				std::cout << "pf[" << i << "]: " << prev.first << ", " << prev.second.first << std::endl;

				for (auto cur : m_wordPosFreqs[word]) {
					std::cout << "cf[" << i << "]: " 
						<< cur.first << ", " << cur.second << std::endl;
					// cur.first: POS, cur.second: freq
					double logProb = 0.0;
					logProb += std::log(prev.second.first);
					std::cout << "m_succFreqs: " << m_succFreqs[prev.first][cur.first]
						<< ", posFreq: " << m_posFreqs[prev.first] << std::endl;
					logProb += std::log(m_succFreqs[prev.first][cur.first] / (double) m_posFreqs[prev.first]);
					//// logProb += std::log(m_wordPosFreqs[word][cur.first] / (double) m_wordFreqs[word]);
					std::cout << "cur.second: " << cur.second << ", wordFreq: " << m_wordFreqs[word] << std::endl;
					logProb += std::log(cur.second / (double) m_wordFreqs[word]);

					// update
					std::cout << "exp: " << std::exp(logProb) << std::endl;
					// std::cout << "score: " << scores[i][cur.first].first << std::endl;
					if (std::exp(logProb) > scores[i][cur.first].first) {
						scores[i].emplace(cur.first, std::make_pair(std::exp(logProb), prev.first));
					}
				}
			}
			
			// word, pos
			std::vector<std::pair<std::string, std::string>> chk;
			std::string cur_pos = END_SYMBOL;
			// back trace
			for (int i = sequenceLength - 1; i >= 0; i--) {
				chk.emplace_back(sentence[i].first, scores[i][cur_pos].second);
				cur_pos = scores[i][cur_pos].second;
			}
			
			// test
			// std::cout << "---" << std::endl;
			// for (int i = 0; i < sentence.size() - 1; i++) {
			// 	std::cout << "ans: " << sentence.at(i).first << ", " << sentence.at(i).second << std::endl; 
			// 	std::cout << "chk: " << chk.at(sentence.size() - i).first 
			// 						<< ", " << chk.at(sentence.size() - i).second << std::endl; 
			// }
		}
	}
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

