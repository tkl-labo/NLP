#pragma once

#include <vector>
#include <unordered_map>

namespace nlp
{
typedef std::unordered_map<std::string,
			std::unordered_map<std::string, int>> SuccFreqList;

class Tagger
{
	int m_mode;
	SuccFreqList m_succFreqs;
	std::unordered_map<std::string, long> m_freqs;

public:
	Tagger(const int mode) : m_mode(mode) {};
	virtual ~Tagger() {};
	void init();
	void train(const std::string &training);
	void test(const std::string &testing);
	void showAllProbabilities();
};
}
