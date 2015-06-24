#include <iostream>
#include <wchar.h>
#include <memory>
#include "stopwatch.h"
#include "n_gram.h"

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] 
            << " training_data [N = 2]" << std::endl;
        return 0;
    }
    const int N = (argc >= 3) ? std::atoi(argv[2]) : 2;
    
    setlocale(LC_ALL, "ja_JP.UTF-8");
    
    util::Stopwatch<std::chrono::milliseconds> sw;
    nlp::NGram ngram(argv[1]);
    ngram.train(N);
    sw.stop();
    
    ngram.showAllProbabilities();
    
    std::cout << "Elapsed: " << sw.showElapsedTime().c_str() << std::endl;

    return 0;
}

