#include <iostream>
#include "ngram.h"

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " training_data_file test_data_file N(default 2)" << std::endl;
    }
    std::string train_data_file = argv[1];
    std::string test_data_file = argv[2];
    // TODO: N == 1に対応
    int N = argc < 3 ? 2 : std::atoi(argv[3]);

    NGram ngram(N);
    ngram.train(train_data_file);

//    ngram.showProbabilities();

    ngram.test(test_data_file);
    double perplexity = ngram.calcPerplexity(test_data_file);

    std::cout << "Perplexity: " << perplexity << std::endl;

    return 0;
}
