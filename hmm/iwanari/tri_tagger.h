#pragma once

#include <vector>
#include <unordered_map>
#include "tagger.h"

namespace nlp
{
typedef std::unordered_map<std::string,
			std::unordered_map<std::string, 
				std::unordered_map<std::string, long>>> TriFreqList;
typedef std::unordered_map<std::string, 
				std::pair<double, 
					std::pair<std::string, std::string>>> TriScoreList;

class TriTagger : public Tagger
{
protected:
	TriFreqList m_triSuccFreqs;	// count(pos_2 | pos_0, pos_1)
	std::unordered_map<std::string, 
		std::unordered_map<std::string, long>> m_triPosFreqs;	// count(pos_0, pos_1)

public:
	TriTagger(const int mode, const bool debug = false)
		: Tagger(mode, debug) {};
	virtual ~TriTagger() {};
	void init();
	void train(const std::string &training);
	void test(const std::string &testing);
	void showSuccProbs();
	void showWordPosProbs();
	void showAllProbs();
	inline double getSuccProb(const std::string pos0, const std::string pos1, const std::string pos2) {
		// NOTE: a speed up way
		// TODO: interpolation 
		if (m_triSuccFreqs.find(pos0) != m_triSuccFreqs.end() 
				&& m_triSuccFreqs[pos0].find(pos1) != m_triSuccFreqs[pos0].end())
			return (m_triSuccFreqs[pos0][pos1][pos2] + 1) 
					/ (double) (m_triPosFreqs[pos0][pos1] + m_triPosFreqs.size() + 1);
		// UNKNOWN_SEQUENCE
		return 1 / (double) (m_triPosFreqs[pos0][pos1] + m_triPosFreqs.size() + 1);
	}

protected:
	void forwardTest(std::ifstream &input_file);
	void viterbiTest(std::ifstream &input_file);
	void forwardPropagate(
		std::vector<std::pair<std::string, std::string>> &sentence,
		std::vector<TriScoreList> &scores);
	std::vector<std::pair<std::string, std::string>>
		nextSenetence(std::ifstream &stream);
};
}
