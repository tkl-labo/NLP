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
	m_wordFreqs.clear();
	m_posFreqs.clear();
	m_wordFreqs.clear();
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
			m_wordPosFreqs[END_SYMBOL][END_SYMBOL]++;
			m_wordFreqs[END_SYMBOL]++;
			
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
			logLikelihood += std::log(getSuccProb(cur_pos, END_SYMBOL));
			
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
			logLikelihood += std::log(getSuccProb(cur_pos, rows[1]));
			logLikelihood += std::log(getWordPosProb(rows[0], rows[1]));
			std::cout << rows[0] << " ";
			cur_pos = rows[1];
		}
	}
}

void Tagger::forwardPropagate(
	std::vector<std::pair<std::string, std::string>> &sentence,
	std::vector<ScoreList> &scores)
{
	
	// start state
	scores[0].emplace(START_SYMBOL, std::make_pair(1.0, ""));
	for (int i = 1; i < sentence.size(); i++) {
		std::string word = sentence.at(i).first;
		
		// prev.first: POS, prev.second: <prob, previous POS>
		for (auto prev : scores[i - 1]) {
			
			// NOTE: using wordPosFreq may be faster than using m_succFreqs
			// because of the number of combination
			// auto list = m_succFreqs[prev.first];
			auto list = m_wordPosFreqs[word].size() != 0 ? 
				m_wordPosFreqs[word] : m_succFreqs[prev.first];
			// cur.first: POS, cur.second: freq
			for (auto cur : list) {
				double logProb = 0.0;
				logProb += std::log(prev.second.first);
				logProb += std::log(getSuccProb(prev.first, cur.first));
				logProb += std::log(getWordPosProb(word, cur.first));
				
				// update
				if (std::exp(logProb) >= scores[i][cur.first].first)
					scores[i][cur.first] = std::make_pair(std::exp(logProb), prev.first);
			}
		}
	}
}

void Tagger::viterbiTest(std::ifstream &input_file)
{
	// get sentence one by one
	// sentence contains (word, ans) pair
	std::vector<std::pair<std::string, std::string>> sentence;
	WrongList wrongList;
	long counter = 0;
	long incorrect = 0;
	while((sentence = nextSenetence(input_file)).size() != 0) {
		sentence.insert(sentence.begin(), std::make_pair(START_SYMBOL, START_SYMBOL));
		sentence.emplace_back(END_SYMBOL, END_SYMBOL);
		
		// current POS -> (prob, previous POS)
		std::vector<ScoreList> scores(sentence.size());
		
		forwardPropagate(sentence, scores);
		
		// word, pos
		std::vector<std::string> chk;
		std::string cur_pos = END_SYMBOL;

		// back trace
		for (int i = sentence.size() - 1; i >= 0; i--) {
			chk.push_back(scores[i][cur_pos].second);
			cur_pos = scores[i][cur_pos].second;
		}

		// test
		for (int i = 1; i < sentence.size() - 1; i++) {
			const std::string word = sentence.at(i).first;
			const std::string ansPos = sentence.at(i).second;
			const std::string tesPos = chk.at(sentence.size() - i - 2);
			if (ansPos != tesPos) {
				incorrect++;
				std::cout << "\x1b[31m";
				wrongList[std::make_pair(ansPos, tesPos)]++;
			}
			std::cout << word << " (ANS: " 
						<< ansPos << ", TES: " 
						<< tesPos << ") ";
			std::cout << "\x1b[39m";
			counter++;
		}
	}
	std::cout << std::endl;
	for (auto wrong : wrongList) {
		std::cout << "count (ANS: " << wrong.first.first << " -> TES: " << wrong.first.second << ") = " << wrong.second << std::endl;
	}
	// std::cout << "word count " << counter << std::endl;
	std::cout << "correct: " << (counter - incorrect) / (double) (counter) * 100 << "%" << std::endl;
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
				<< getSuccProb(f.first, s.first)
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
				<< getWordPosProb(f.first, s.first)
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

