#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include "n_gram.h"
using namespace nlp;

const std::string START_SYMBOL = "<s>";
const std::string END_SYMBOL = "</s>";
const std::string END_SYMBOL_IN_CORPUS = "EOS";
const std::string DELIME_IN_CORPUS = "\t";
const double epsilon = 0.000001;

/* ======== util ========= */
std::string joinString(const std::vector<std::string> &strings, 
    const std::string &delim)
{
    std::string joined_string;
    if (strings.size() > 0) {
        joined_string = strings[0];
        for (int i = 1; i < strings.size(); i++) {
            joined_string += delim;
            joined_string += strings[i];
        }
    }
    return joined_string;
}

std::string getFirstString(const std::string &str, const std::string &delim)
{
    size_t fp; // current position, found position
    
    if ((fp =  str.find(delim)) != std::string::npos)
        return str.substr(0, fp);
    
    return str; // EOS has no following delim
}

std::vector<std::string> splitString(const std::string &str, const std::string &delim)
{
    std::vector<std::string> tokens;

    size_t cp, fp; // current position, found position
    const size_t dsize = delim.size();

    for (cp = 0; (fp = str.find(delim, cp)) != std::string::npos; cp = fp + dsize)
        tokens.emplace_back(str, cp, fp - cp);

    tokens.emplace_back(str, cp, str.size() - cp);
    return tokens;
}
/* ======== /util ========= */


/* ======== NGram::NODE ======== */
bool NGram::Node::insertKey(const std::string& key)
{
    m_total_freq++;
    auto it = m_freqs.find(key);
    if (it != m_freqs.end()) {
        it->second++;
        return false;
    }
    else {
        m_freqs.emplace(key, 1);
        return true;
    }
}

double NGram::Node::prob(const std::string &key)
{
    if (m_total_freq == 0)
        return 0.0;
    return m_freqs[key] / (double) m_total_freq;
}
/* ======== /NGram::NODE ======== */


/* ======== NGram ======== */

void NGram::train(const std::string &training)
{
    m_num_of_ngrams = 0;
    m_root.clear();
    
    std::cout << "training..." << std::endl;
    
    std::ifstream input_file(training);
    if (!input_file) {
        std::cerr << "ERROR: No such file (" 
                    << training << ")" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    constructTree(input_file);
    input_file.close();
    
    std::cout << "trained!" << std::endl;
}

NGramKey createNGramKey(const int n)
{
    return NGramKey(n - 1, START_SYMBOL);
}

void NGram::insertKey(const NGramKey &key, const std::string &word)
{
    auto it = m_root.find(key);
    if (it != m_root.end()){
        // if it is a new key, increment the num of ngrams
        if (it->second.insertKey(word))
            m_num_of_ngrams++;
    } 
    else {
        NGram::Node node;
        // if it is a new key, increment the num of ngrams
        if (node.insertKey(word))
            m_num_of_ngrams++;
        m_root.emplace(key, node);
    }
}

void NGram::constructTree(std::istream &stream)
{
    std::string str;
    
    NGramKey key = createNGramKey(m_N);
    while(std::getline(stream, str)) {
        // this program assumes processed corpus
        std::string word = getFirstString(str, DELIME_IN_CORPUS);
        
        // if the word is the line end symbol (EOS)
        if (word == END_SYMBOL_IN_CORPUS)
            word = END_SYMBOL;
        
        // insert new key and following word
        insertKey(key, word);
        
        // if the current word is the end of line, initialize the key
        if (word == END_SYMBOL) {
            key = createNGramKey(m_N);
        }
        else {
            // erase the front element
            key.erase(key.begin());
            // add word
            key.push_back(word);
        }
    }
}

NGram::Node NGram::findByKey(const NGramKey &key)
{
    auto it = m_root.find(key);
    if (it != m_root.end()){
        return it->second;
    } 
    else {
        NGram::Node node;
        return node;
    }
}

double NGram::calcPerplexity(std::istream &stream)
{
    std::string str;
    double perplexity = 1.0;
    
    NGramKey key = createNGramKey(m_N);
    while(std::getline(stream, str)) {
        // this program assumes processed corpus
        std::string word = getFirstString(str, DELIME_IN_CORPUS);
        
        // if the word is the line end symbol (EOS)
        if (word == END_SYMBOL_IN_CORPUS)
            word = END_SYMBOL;
        
        // find new key and following word
        // without smoothing, it often returns 0
        double p = findByKey(key).prob(word);
        perplexity *= p;
        
        // if the current word is the end of line, initialize the key
        if (word == END_SYMBOL) {
            key = createNGramKey(m_N);
        }
        else {
            // erase the front element
            key.erase(key.begin());
            // add word
            key.push_back(word);
        }
    }
    // if (perplexity < epsilon)
    //     return 1 / epsilon;
    
    perplexity = std::pow(1.0 / perplexity, 1.0 / m_num_of_ngrams);
    
    return perplexity;
}

double NGram::calcPerplexity(const std::string &testing)
{
    std::cout << "calc perplexity..." << std::endl;
    
    std::ifstream input_file(testing);
    if (!input_file) {
        std::cerr << "ERROR: No such file (" 
                    << testing << ")" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    
    double perplexity = calcPerplexity(input_file);
    input_file.close();
    std::cout << "calculated!" << std::endl;
    
    return perplexity;
}

// debug function
void NGram::showAllProbabilities()
{
    for (auto pair : m_root) {
        for (auto freq : pair.second.m_freqs) {
            std::cout << "P ( " << freq.first << " | ";
            
            std::cout << joinString(pair.first, ", ");
            std::cout << " ) = ";
            
            std::cout << pair.second.prob(freq.first) << std::endl;
        }
    }
}

// debug function
void NGram::showAllNgrams()
{
    for (auto pair : m_root) {
        std::cout << "Key: ";
        std::cout << joinString(pair.first, ",");
        std::cout << " Val: ";
        for (auto freq : pair.second.m_freqs)
            std::cout << freq.first << ", " << freq.second << std::endl;
    }
}
/* ======== /NGram ======== */

