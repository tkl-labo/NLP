//
// 形態素解析結果のcdrファイルを、
// 各単語をスペースで、各文を改行で区切る文に変換するスクリプト
//
#include "common.h"

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " cdr_file" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::string cdr_file = argv[1];
    std::ifstream input(cdr_file);
    if (!input) {
        std::cerr << "cannot find cdr file." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::string line;

    while (std::getline(input, line))
    {
        std::string surface = line.substr(0, line.find(CDR_DELIMITER));
        if (surface == SENTENCE_END_SYMBOL)
        {
            std::cout << std::endl;
        }
        else
        {
            std::cout << surface << SENTENCE_DELIMITER;
        }
    }
}