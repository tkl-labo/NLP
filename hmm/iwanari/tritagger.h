#pragma once

#include <vector>
#include <unordered_map>
#include "tagger.h"

namespace nlp
{
typedef std::unordered_map<std::string,
			std::unordered_map<std::string, 
				std::unordered_map<std::string, long>>> TriFreqList;

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
	virtual void init();
	virtual void train(const std::string &training);
	virtual void showSuccProbs();
	inline double getSuccProb(const std::string pos0, const std::string pos1, const std::string pos2) {
		// double prob = 0;
		
		// interpolation
		// NOTE: a speed up way
		if (m_triPosFreqs.find(pos0) != m_triPosFreqs.end())
			if (m_triPosFreqs[pos0].find(pos1) != m_triPosFreqs[pos0].end())
				if (m_triSuccFreqs[pos0][pos1].find(pos2) != m_triSuccFreqs[pos0][pos1].end())
						return (m_triSuccFreqs[pos0][pos1][pos2] + SMOOTHING_COEFFICIENT) 
								/ (double) (m_triPosFreqs[pos0][pos1] 
									+ (m_triPosFreqs.size() + 1) * SMOOTHING_COEFFICIENT);
		
		// if (m_posFreqs.find(pos1) != m_posFreqs.end())
		// 	if (m_succFreqs[pos1].find(pos2) != m_succFreqs[pos1].end())
		// 		prob += lambda1 * (m_succFreqs[pos1][pos2] + 1)
		// 			/ (double) (m_posFreqs[pos1] + m_posFreqs.size() + 1);
		//
		// if (m_posFreqs.find(pos2) != m_posFreqs.end())
		// 	return prob + lambda2 * (m_posFreqs[pos2] + 1) 
		// 			/ (double) (m_totalFreqs + m_posFreqs.size() + 1);
		
		// UNKNOWN_SEQUENCE
		return SMOOTHING_COEFFICIENT / (double) ((m_triPosFreqs.size() + 1) * SMOOTHING_COEFFICIENT);
	}

protected:
	virtual void forwardTest(std::ifstream &input_file);
	virtual void forwardPropagate(
		std::vector<std::pair<std::string, std::string>> &sentence,
		std::vector<ScoreList> &scores);
};
}
