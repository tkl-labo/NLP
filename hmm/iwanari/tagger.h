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
	inline double getSuccProb(const std::string pos0, const std::string pos1) {
		return m_succFreqs[pos0][pos1] / (double) m_posFreqs[pos0];
	}
	inline double getWordPosProb(const std::string word, const std::string pos) {
		return (m_wordPosFreqs[word][pos] + 1)
			/ (double) (m_wordFreqs[word] + m_wordFreqs.size() + 1);
	}
	
private:
	void forwardTest(std::ifstream &input_file);
	void viterbiTest(std::ifstream &input_file);
	void forwardPropagate(
		std::vector<std::pair<std::string, std::string>> &sentence,
		std::vector<ScoreList> &scores);

};
}
