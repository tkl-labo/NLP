#pragma once
#include <iostream>
#include <vector>

namespace nlp
{
class NGram
{
private:
    std::string m_training;
    int m_num_of_pairs;
    
public:
    NGram(const std::string training) 
    : m_num_of_pairs(0) {
        m_training = training;
    }
    virtual ~NGram() {}
    
    std::string trainingFileName() { return m_training; }
    void train(const int n);

private:
    void readFile();
};
}
