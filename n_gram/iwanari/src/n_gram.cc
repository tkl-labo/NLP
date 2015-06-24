#include <iostream>
#include <fstream>
#include <string>
#include "n_gram.h"

using namespace nlp;

void NGram::train(const int n)
{
    readFile();
    
}

void NGram::readFile()
{
    std::ifstream input_file;
    input_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        input_file.open(m_training);
        input_file.close();
    }
    catch (std::ifstream::failure e) {
        std::cerr << "Exception opening/reading/closing file" << std::endl;
    }
}
