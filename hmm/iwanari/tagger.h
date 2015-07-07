#pragma once

#include <vector>
#include <unordered_map>

namespace std {
template <>
class hash<std::pair<std::string, std::string>> {
public:
	size_t operator()(const std::pair<std::string, std::string>& x) const {
		return hash<std::string>()(x.first) 
					^ hash<std::string>()(x.second);
	}
};
}

namespace nlp
{
typedef std::unordered_map<std::string,
			std::unordered_map<std::string, long>> PairFreqList;
typedef std::unordered_map<std::string, 
				std::pair<double, std::string>> ScoreList;
typedef std::unordered_map<std::pair<std::string, std::string>, long> WrongList;


class Tagger
{
protected:
	int m_mode;
	int m_debug;
	PairFreqList m_succFreqs;	// count(pos_1 | pos_0)
	PairFreqList m_wordPosFreqs;	// count(word | pos)
	std::unordered_map<std::string, long> m_posFreqs;	// count(pos_0)
	std::unordered_map<std::string, long> m_wordFreqs;	// count(word)

public:
	Tagger(const int mode, const bool debug = false)
		: m_mode(mode), m_debug(debug) {};
	virtual ~Tagger() {};
	void init();
	virtual void train(const std::string &training);
	virtual void test(const std::string &testing);
	virtual void showSuccProbs();
	virtual void showWordPosProbs();
	virtual void showAllProbs();
	inline double getSuccProb(const std::string pos0, const std::string pos1) {
		return m_succFreqs[pos0][pos1] / (double) m_posFreqs[pos0];
	}
	inline double getWordPosProb(const std::string word, const std::string pos) {
		// NOTE: a speed up way
		if (m_wordFreqs.find(word) != m_wordFreqs.end())
			return (m_wordPosFreqs[word][pos] + 1)
				/ (double) (m_wordFreqs[word] + m_wordFreqs.size() + 1);
		// UNKNOWN_WORD
		return 1 / (double) (m_wordFreqs.size() + 1);
	}

protected:
    std::string joinString(const std::vector<std::string> &strings, 
         const std::string &delim);
    std::string getFirstString(const std::string &str, 
         const std::string &delim);
    std::vector<std::string> splitString(const std::string &str,
                const std::string &delim);
	virtual void forwardTest(std::ifstream &input_file);
	virtual void viterbiTest(std::ifstream &input_file);
	virtual void forwardPropagate(
		std::vector<std::pair<std::string, std::string>> &sentence,
		std::vector<ScoreList> &scores);
	std::vector<std::pair<std::string, std::string>>
		nextSenetence(std::ifstream &stream);
};
}
