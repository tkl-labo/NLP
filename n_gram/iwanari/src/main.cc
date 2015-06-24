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
    nlp::NGram ngram((argc >= 4) ? std::atoi(argv[3]) : 2);
    ngram.train(argv[1]);
    sw.stop();
    
    std::cout << "Elapsed (training): " 
        << sw.showElapsedTime().c_str() << std::endl;
    // debug
    // ngram.showAllProbabilities();
    
    sw.start();
    std::cout << "perplexity: " 
        << ngram.calcPerplexity(argv[2]) << std::endl;
    sw.stop();
    
    std::cout << "Elapsed (perplexity): " 
        << sw.showElapsedTime().c_str() << std::endl;
    
    

    return 0;
}

