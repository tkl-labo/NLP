#include <iostream>
#include <fstream>
#include "tagger.h"
using namespace nlp;

void Tagger::train(const std::string &training)
{
    std::cout << "training..." << std::endl;
    std::ifstream input_file(training);
    if (!input_file) {
        std::cerr << "ERROR: No such file (" 
                    << training << ")" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    input_file.close();
    std::cout << "trained!" << std::endl;
}

void Tagger::test(const std::string &testing)
{
}
