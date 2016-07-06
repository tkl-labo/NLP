//
// perplexityをカウントするスクリプト
//
#include "utility.h"
#include "common.h"
#include <cmath>
#include <unordered_set>

int getNgramCount(const std::string& ngram_file, const std::string& ngram);
void countVocabularySize(const std::string& ngram_file, int *v, int *n);

int main(int argc, char** argv)
{
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " N ngram_file n-1gram_file test_file" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    int N = std::stoi(argv[1]);
    std::string ngram_file = argv[2];
    std::string n_1gram_file = argv[3];
    std::string test_file = argv[4];

    std::ifstream ngram_input(ngram_file);
    if (!ngram_input) {
        std::cerr << "cannot find ngram file." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::ifstream n_1gram_input(n_1gram_file);
    if (!n_1gram_input) {
        std::cerr << "cannot find n-1gram file." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::ifstream test_input(test_file);
    if (!test_input) {
        std::cerr << "cannot find test file." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // V: vocabulary size, C: number of ngrams
    int V, C;
    countVocabularySize(ngram_file, &V, &C);
    std::cout << "vocabulary: " << V << ", ";
    std::cout << N << "-grams: " << C << std::endl;

    // perplexity計算
    double total_pp = 1.0;
    std::string line;
    // testファイルの各行でloopを回す
    while (std::getline(test_input, line))
    {
        // 一文に含まれる単語をvectorに
        auto words = split(line, SENTENCE_DELIMITER);
        int words_size = words.size();
        // N-1個<s>を先頭に追加
        auto words_begin = words.begin();
        words.insert(words_begin, N-1, NGRAM_START_SYMBOL);
        // 単語数分loop
        for (int i = 0; i < words_size; i++)
        {
            // ngram, n-1gramを切り出す
            auto first = words.begin() + i;
            auto last = first + N;
            auto n_1gram_last = last - 1;
            std::string ngram = join(first, last, SENTENCE_DELIMITER);
            std::string n_1gram = join(first, n_1gram_last, SENTENCE_DELIMITER);
            // 学習したデータからcとc_1を取得（c: ngramのcount, c_1: n-1gramのcount）
            int c = 0;
            int c_1 = 0;
            c = getNgramCount(ngram_file, ngram);
            c_1 = getNgramCount(n_1gram_file, n_1gram);
            double p = (double) (c + 1) / (c_1 + V);
            double pp = std::pow(1.0 / p, 1.0 / C);
            total_pp *= pp;
            std::cout << *(last-1) << SENTENCE_DELIMITER << pp << std::endl;
        }
        std::cout << "perplexity: " << total_pp << std::endl;
    }
}


//
// ngramとその出現回数の組を学習したファイルから、ngramの出現回数を取得する関数
// @param ngram_file: ngramとその出現回数の組を学習したファイル
// @param ngram: 出現回数を取得したいngram
//
int getNgramCount(const std::string& ngram_file, const std::string& ngram)
{
    // ngramのカウント
    std::ifstream ngram_input(ngram_file);
    std::string ngram_line;
    while (std::getline(ngram_input, ngram_line))
    {
        std::size_t found = ngram_line.find(ngram);
        if (found != std::string::npos)
            return std::stoi(split(ngram_line, SENTENCE_DELIMITER)[0]);
    }
    return 0;
}


//
// Vocabularyのsize（uniqueな単語数）を数える関数
// @param ngram_file: ngramとその出現回数を記述したファイル
// @param V: vocabulary size
// @param C: number of ngrams
//
void countVocabularySize(const std::string& ngram_file, int *V, int *C)
{
    *V = 0;
    *C = 0;
    std::unordered_set<std::string> vocabulary;
    std::ifstream ngram_input(ngram_file);
    std::string ngram_line;
    while (std::getline(ngram_input, ngram_line))
    {
        std::vector<std::string> ngram_count = split(ngram_line, SENTENCE_DELIMITER);
        // ngramの末尾を取得する
        std::string target_word = ngram_count.back();
        vocabulary.insert(target_word);
        *C += 1;
    }
    *V = vocabulary.size();
}