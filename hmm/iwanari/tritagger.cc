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
	std::string cur_pos0 = START_SYMBOL;
	std::string cur_pos1 = START_SYMBOL;

	while(std::getline(input_file, line)) {
		// rows[0]: word, rows[1]: POS
		std::vector<std::string> rows 
			= splitString(line, DELIME_IN_CORPUS);
		
		// count the frequency of POS
		m_triPosFreqs[cur_pos0][cur_pos1]++;
		
		// the end of sentence
		if (rows.size() == 1) {
			// added end symbol
			m_triSuccFreqs[cur_pos0][cur_pos1][END_SYMBOL]++;
			m_wordPosFreqs[END_SYMBOL][END_SYMBOL]++;
			m_wordFreqs[END_SYMBOL]++;
			
			// init
			cur_pos0 = START_SYMBOL;
			cur_pos1 = START_SYMBOL;
		}
		else {
			m_triSuccFreqs[cur_pos0][cur_pos1][rows[1]]++;
			cur_pos0 = cur_pos1;
			cur_pos1 = rows[1];
			
			// count the frequency of (word, pos) pair
			m_wordPosFreqs[rows[0]][rows[1]]++;
			m_wordFreqs[rows[0]]++;
		}
	}

	input_file.close();
	std::cout << "trained!" << std::endl;
}

std::vector<std::pair<std::string, std::string>> 
TriTagger::nextSenetence(std::ifstream &stream)
{
	std::string line;
	// sentence contains (word, ans) pair
	std::vector<std::pair<std::string, std::string>> sentence;
	while(std::getline(stream, line)) {
		// rows[0]: word, rows[1]: POS
		std::vector<std::string> rows 
			= splitString(line, DELIME_IN_CORPUS);
		
		// the end of sentence
		if (rows.size() == 1)
			return sentence;
		
		sentence.emplace_back(rows[0], rows[1]);
	}
	return sentence;
}

void TriTagger::forwardTest(std::ifstream &input_file)
{
	std::string line;
	std::string cur_pos0 = START_SYMBOL;
	std::string cur_pos1 = START_SYMBOL;
	double logLikelihood = 0.0;
	
	while(std::getline(input_file, line)) {
		// rows[0]: word, rows[1]: POS
		std::vector<std::string> rows 
			= splitString(line, DELIME_IN_CORPUS);
		
		// the end of sentence
		if (rows.size() == 1) {
			// TODO: is this necessary?
			logLikelihood += std::log(getSuccProb(cur_pos0, cur_pos1, END_SYMBOL));
			
			// calculate likelihood
			std::cout << std::endl;
			std::cout << "Likelihood: " << std::exp(logLikelihood);
			
			// init
			std::cout << std::endl << std::endl;
			cur_pos0 = START_SYMBOL;
			cur_pos1 = START_SYMBOL;
			logLikelihood = 0.0;
		}
		else {
			// calculate likelihood in log
			logLikelihood += std::log(getSuccProb(cur_pos0, cur_pos1, rows[1]));
			logLikelihood += std::log(getWordPosProb(rows[0], rows[1]));
			std::cout << rows[0] << " ";
			cur_pos0 = cur_pos1;
			cur_pos1 = rows[1];
		}
	}
}

void TriTagger::forwardPropagate(
	std::vector<std::pair<std::string, std::string>> &sentence,
	std::vector<ScoreList> &scores)
{
	
	// start state
	scores[0].emplace(START_SYMBOL, std::make_pair(1.0, ""));
	scores[1].emplace(START_SYMBOL, std::make_pair(1.0, START_SYMBOL));
	for (int i = 2; i < sentence.size(); i++) {
		std::string word = sentence.at(i).first;
		
		// prevprev.first: POS, prevprev.second: <prob, previous POS>
		for (auto prevprev : scores[i - 2]) {
			
			// prev.first: POS, prev.second: <prob, previous POS>
			for (auto prev : m_triSuccFreqs[prevprev.first]) {
				// NOTE: using wordPosFreq may be faster than using m_triSuccFreqs
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

void TriTagger::viterbiTest(std::ifstream &input_file)
{
	// get sentence one by one
	// sentence contains (word, ans) pair
	std::vector<std::pair<std::string, std::string>> sentence;
	WrongList wrongList;
	long counter = 0;
	long incorrect = 0;
	while((sentence = nextSenetence(input_file)).size() != 0) {
		sentence.emplace(sentence.begin(), START_SYMBOL, START_SYMBOL);
		sentence.emplace(sentence.begin(), START_SYMBOL, START_SYMBOL);
		sentence.emplace_back(END_SYMBOL, END_SYMBOL);
		
		// current POS -> (prob, previous POS)
		std::vector<ScoreList> scores(sentence.size());
		
		forwardPropagate(sentence, scores);
		
		// word, pos
		std::vector<std::string> chk;
		std::string cur_pos = END_SYMBOL;

		// // back trace
		// for (int i = sentence.size() - 1; i >= 0; i--) {
		// 	chk.emplace_back(scores[i][cur_pos].second);
		// 	cur_pos = scores[i][cur_pos].second;
		// }

		// // test
		// for (int i = 1; i < sentence.size() - 1; i++) {
		// 	const std::string word = sentence.at(i).first;
		// 	const std::string ansPos = sentence.at(i).second;
		// 	const std::string tesPos = chk.at(sentence.size() - i - 2);
		// 	if (ansPos != tesPos) {
		// 		incorrect++;
		// 		std::cout << "\x1b[31m";
		// 		if (m_debug)
		// 			wrongList[std::make_pair(ansPos, tesPos)]++;
		// 	}
		// 	std::cout << word << " (ANS: " 
		// 				<< ansPos << ", TES: " 
		// 				<< tesPos << ") ";
		// 	std::cout << "\x1b[39m";
		// 	counter++;
		// }
	}
	std::cout << std::endl;
	// if (m_debug) {
	// 	for (auto wrong : wrongList) {
	// 		std::cout << "count (ANS: " << wrong.first.first 
	// 			<< " -> TES: " << wrong.first.second << ") = " << wrong.second << std::endl;
	// 	}
	// }
	// 
	// std::cout << "word count " << counter << std::endl;
	std::cout << "correct: " << (counter - incorrect) / (double) (counter) * 100 << "%" << std::endl;
}

void TriTagger::test(const std::string &testing)
{
	std::cout << "testing..." << std::endl;
	std::ifstream input_file(testing);
	if (!input_file) {
		std::cerr << "ERROR: No such testing file (" 
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

void TriTagger::showWordPosProbs()
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
// ===== /for debug =====


