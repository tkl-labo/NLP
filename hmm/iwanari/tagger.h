#pragma once

#include <vector>

namespace nlp
{
class Tagger
{
    int m_mode;
public:
    Tagger(const int mode) : m_mode(mode) {};
    virtual ~Tagger() {};
    void train(const std::string &training);
    void test(const std::string &testing);
};
}
