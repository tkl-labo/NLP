#pragma once

#include <vector>
#include <unordered_map>

namespace nlp
{
typedef std::unordered_map<std::string,
			std::unordered_map<std::string, long>> PairFreqList;
typedef std::unordered_map<std::string, 
				std::pair<double, std::string>> ScoreList;

class Tagger
{
	int m_mode;
	PairFreqList m_succFreqs;	// count(pos_1 | pos_0)
	PairFreqList m_wordPosFreqs;	// count(word | pos)
	std::unordered_map<std::string, long> m_posFreqs;	// count(pos_0)
	std::unordered_map<std::string, long> m_wordFreqs;	// count(word)

public:
	Tagger(const int mode) : m_mode(mode) {};
	virtual ~Tagger() {};
	void init();
	void train(const std::string &training);
	void test(const std::string &testing);
	void showSuccProbs();
	void showWordPosProbs();
	void showAllProbs();
private:
	void forwardTest(std::ifstream &input_file);
	void viterbiTest(std::ifstream &input_file);

};
}
