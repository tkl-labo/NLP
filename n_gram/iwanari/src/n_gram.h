#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>

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
        bool insertKey(const std::string& key);
    };

    int m_num_of_ngrams;
    int m_N;
    std::string m_training;
    std::map<NGramKey, NGram::Node> m_root;

public:
    NGram(const std::string training) 
        : m_training(training) { }
    virtual ~NGram() {}
    
    std::string trainingFileName() { return m_training; }
    void train(const int N);
    void showAllProbabilities();
    void showAllNgrams();

private:
    void readFile();
    void constructTree(std::istream &stream);
};
}
