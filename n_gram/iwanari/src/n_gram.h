#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>

namespace nlp
{

typedef std::vector<std::string> NGramKey;

class NGram
{
private:
    struct Node {
        int m_total_freq;
        std::unordered_map<std::string, int> m_freqs;
        Node() : m_total_freq(0) {}
        bool insertKey(const std::string &key);
        std::string findMaxiumLikelihoodKey();
    };

    int m_num_of_ngrams;
    int m_N;
    std::map<NGramKey, NGram::Node> m_root;
    std::set<std::string> m_vocab;

public:
    NGram(const int N) 
        : m_N(N) { }
    virtual ~NGram() {}
    
    void train(const std::string &training);
    double calcPerplexity(const std::string &testing);
    double calcPerplexity(std::istream &stream);
    double prob(const NGram::Node &node, const std::string &key);
    
    std::string genMaximumLikelihoodString(const std::string &seed, const int N);
    
    // debug functions
    void showAllProbabilities();
    void showAllNgrams();

private:
    NGram::Node findByKey(const NGramKey &key);
    void insertKey(const NGramKey &key, const std::string &word);
    void constructTree(std::istream &stream);
};
}
