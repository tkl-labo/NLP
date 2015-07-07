#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include "nlp.h"
#include "tritagger.h"
using namespace nlp;

void TriTagger::init()
{
	m_triSuccFreqs.clear();
	m_wordFreqs.clear();
	m_triPosFreqs.clear();
	m_wordFreqs.clear();
}

void TriTagger::train(const std::string &training)
{
	init();
	
	std::cout << "training..." << std::endl;
	std::ifstream input_file(training);
	if (!input_file) {
		std::cerr << "ERROR: No such training file (" 
			<< training << ")" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	std::string line;
	std::pair<std::string, std::string> cur_pos
		= {START_SYMBOL, START_SYMBOL};

	while(std::getline(input_file, line)) {
		// rows[0]: word, rows[1]: POS
		std::vector<std::string> rows 
			= splitString(line, DELIME_IN_CORPUS);
		
		// count the frequency of POS
		m_posFreqs[cur_pos.second]++;
		m_triPosFreqs[cur_pos.first][cur_pos.second]++;
		
		// the end of sentence
		if (rows.size() == 1) {
			// added end symbol
			m_succFreqs[cur_pos.second][END_SYMBOL]++;
			m_triSuccFreqs[cur_pos.first][cur_pos.second][END_SYMBOL]++;
			m_wordPosFreqs[END_SYMBOL][END_SYMBOL]++;
			m_wordFreqs[END_SYMBOL]++;
			
			// init
			cur_pos = std::make_pair(START_SYMBOL, START_SYMBOL);
		}
		else {
			m_succFreqs[cur_pos.second][rows[1]]++;
			m_triSuccFreqs[cur_pos.first][cur_pos.second][rows[1]]++;
			cur_pos = std::make_pair(cur_pos.second, rows[1]);
			
			// count the frequency of (word, pos) pair
			m_wordPosFreqs[rows[0]][rows[1]]++;
			m_wordFreqs[rows[0]]++;
		}
	}

	input_file.close();
	std::cout << "trained!" << std::endl;
}

void TriTagger::forwardTest(std::ifstream &input_file)
{
	std::string line;
	std::pair<std::string, std::string> cur_pos
		= {START_SYMBOL, START_SYMBOL};
	double logLikelihood = 0.0;
	
	while(std::getline(input_file, line)) {
		// rows[0]: word, rows[1]: POS
		std::vector<std::string> rows 
			= splitString(line, DELIME_IN_CORPUS);
		
		// the end of sentence
		if (rows.size() == 1) {
			// TODO: is this necessary?
			logLikelihood += std::log(getSuccProb(cur_pos.first, cur_pos.second, END_SYMBOL));
			
			// calculate likelihood
			std::cout << std::endl;
			std::cout << "Likelihood: " << std::exp(logLikelihood);
			
			// init
			std::cout << std::endl << std::endl;
			cur_pos = std::make_pair(START_SYMBOL, START_SYMBOL);
			logLikelihood = 0.0;
		}
		else {
			// calculate likelihood in log
			logLikelihood += std::log(getSuccProb(cur_pos.first, cur_pos.second, rows[1]));
			logLikelihood += std::log(getWordPosProb(rows[0], rows[1]));
			std::cout << rows[0] << " ";
			cur_pos = std::make_pair(cur_pos.second, rows[1]);
		}
	}
}

void TriTagger::forwardPropagate(
	std::vector<std::pair<std::string, std::string>> &sentence,
	std::vector<ScoreList> &scores)
{
	// initialize senetence and scores
	sentence.emplace(sentence.begin(), START_SYMBOL, START_SYMBOL);
	sentence.emplace(sentence.begin(), START_SYMBOL, START_SYMBOL);
	sentence.emplace_back(END_SYMBOL, END_SYMBOL);
	scores.resize(sentence.size());
	
	// start state
	scores[0].emplace(START_SYMBOL, std::make_pair(1.0, ""));
	scores[1].emplace(START_SYMBOL, std::make_pair(1.0, START_SYMBOL));
	for (int i = 2; i < sentence.size(); i++) {
		std::string word = sentence.at(i).first;
		
		// prevprev.first: POS, prevprev.second: <prob, previous POS>
		for (auto prevprev : scores[i - 2]) {
			
			// prev.first: POS, prev.second: <prob, previous POS>
			for (auto prev : m_triSuccFreqs[prevprev.first]) {
				// NOTE: using wordPosFreq may be faster than using triSuccFreqs
				// because of the number of combination
				// auto list = m_succFreqs[prev.first];
				auto list = m_wordPosFreqs[word].size() != 0 ?
					m_wordPosFreqs[word] : m_triSuccFreqs[prevprev.first][prev.first];
				// cur.first: POS, cur.second: freq
				for (auto cur : list) {
					double logProb = 0.0;
					logProb += std::log(scores[i - 1][prev.first].first);
					logProb += std::log(getSuccProb(prevprev.first, prev.first, cur.first));
					logProb += std::log(getWordPosProb(word, cur.first));
					
					// update
					if (std::exp(logProb) >= scores[i][cur.first].first)
						scores[i][cur.first] = std::make_pair(std::exp(logProb), prev.first);
				}
			}
		}
	}
}

// ===== for debug =====
void TriTagger::showSuccProbs()
{
	for (auto f : m_triSuccFreqs) {
		for (auto s : f.second) {
			for (auto t : f.second) {
				std::cout << "Prob("
					<< t.first << " | "
					<< f.first << ", " 
					<< s.first << ") = " 
					<< getSuccProb(f.first, s.first, t.first)
					<< std::endl;
			}
		}
		std::cout << "=======" << std::endl << std::endl;
	}
}
// ===== /for debug =====


