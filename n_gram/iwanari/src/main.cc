#include <iostream>
#include <wchar.h>
#include <memory>
#include "stopwatch.h"
#include "n_gram.h"

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] 
            << " training_data testing_data [N = 2]" 
            << std::endl;
        return 0;
    }
    
    setlocale(LC_ALL, "ja_JP.UTF-8");
    
    util::Stopwatch<std::chrono::milliseconds> sw;
    // TODO: implement unigram (N = 1)
    nlp::NGram ngram((argc >= 4) ? std::atoi(argv[3]) : 2);
    ngram.train(argv[1]);
    sw.stop();
    
    
    std::cout << "Elapsed (training): " 
        << sw.showElapsedTime().c_str() << std::endl;
    // debug
    // std::cout << "-------" << std::endl;
    // ngram.showAllProbabilities();
    std::cout << "-------" << std::endl;
    
    sw.start();
    double perplexity = ngram.calcPerplexity(argv[2]);
    std::cout << "perplexity: " 
        << perplexity << std::endl;
    sw.stop();

    std::cout << "Elapsed (perplexity): " 
        << sw.showElapsedTime().c_str() << std::endl;
    std::cout << "-------" << std::endl;
    
    while (1) {
        std::cout << "seed: ";
        std::string seed;
        std::cin >> seed;
        std::cout << "N: ";
        int length;
        std::cin >> length;
        
        std::cout << ngram.genMaximumLikelihoodString(seed, length) << std::endl;
    }
    return 0;
}

