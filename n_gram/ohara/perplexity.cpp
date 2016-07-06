//
// perplexityをカウントするスクリプト
//
#include "common.h"
#include <cmath>
#include <unordered_set>

int getNgramCount(const std::string& ngram_file, const std::string& ngram);
void countVocabularySize(const std::string& ngram_file, int *vs, int *vt, int *n);
int countWords(const std::string& test_file);

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

    // VS: vocabulary size, VT: vocabulary type, nof_ngrams: number of ngrams
    int VS, VT, nof_ngrams;
    countVocabularySize(ngram_file, &VS, &VT, &nof_ngrams);
    std::cout << "vocabulary: " << VT << ", ";
    std::cout << N << "-grams: " << nof_ngrams << std::endl;

    // </s>の分で+1
    int nof_words = countWords(test_file) + 1;

    // perplexity計算
    double total_logPP = 0.0;
    std::string line;
    // testファイルの各行でloopを回す
    while (std::getline(test_input, line))
    {
        // 一文に含まれる単語をvectorに
        auto words = split(line, SENTENCE_DELIMITER);
        int words_size = words.size();
        // N-1個<s>を先頭に、1個</s>を末尾に追加
        auto words_begin = words.begin();
        words.insert(words_begin, N-1, NGRAM_START_SYMBOL);
        words.push_back(NGRAM_END_SYMBOL);
        // 単語数分loop
        for (int i = 0; i < (words_size + 1); i++)
        {
            // ngram, n-1gramを切り出す
            auto first = words.begin() + i;
            auto last = first + N;
            auto n_1gram_last = last - 1;
            std::string ngram = join(first, last, SENTENCE_DELIMITER);
            // 学習したデータからcとc_1を取得（c: ngramのcount, c_1: n-1gramのcount）
            int c = 0;
            c = getNgramCount(ngram_file, ngram);
            double p = 0.0;
            if (N != 1) {
                std::string n_1gram = join(first, n_1gram_last, SENTENCE_DELIMITER);
                int c_1 = 0;
                c_1 = getNgramCount(n_1gram_file, n_1gram);
                p = (double) (c + 1) / (c_1 + VT + 1);
            }
            else
            {
                // unigramの場合
                p = (double) (c + 1) / (VS + VT + 1);
            }
            double pp = 1.0 / p;
            total_logPP += log(pp);
            std::cout << *(last-1) << SENTENCE_DELIMITER << pp << std::endl;
        }
        double total_pp = exp(total_logPP / nof_words);
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
        std::vector<std::string> ngram_count = split(ngram_line, SENTENCE_DELIMITER);
        std::string ngram_from_file = join(ngram_count.begin()+1, ngram_count.end(), SENTENCE_DELIMITER);
        if (ngram == ngram_from_file)
            return std::stoi(split(ngram_line, SENTENCE_DELIMITER)[0]);
    }
    return 0;
}


//
// Vocabularyのsize（uniqueな単語数）を数える関数
// @param ngram_file: ngramとその出現回数を記述したファイル
// @param
// @param V: vocabulary type
// @param C: number of ngrams
//
void countVocabularySize(const std::string& ngram_file, int *VS, int *VT, int *nof_ngrams)
{
    *VT = 0;
    *nof_ngrams = 0;
    std::unordered_set<std::string> vocabulary;
    std::ifstream ngram_input(ngram_file);
    std::string ngram_line;
    while (std::getline(ngram_input, ngram_line))
    {
        std::vector<std::string> ngram_count = split(ngram_line, SENTENCE_DELIMITER);
        // ngramの末尾を取得する
        std::string target_word = ngram_count.back();
        if (target_word == NGRAM_START_SYMBOL)
            continue;
        vocabulary.insert(target_word);
        *VS += std::stoi(ngram_count.front());
        *nof_ngrams += 1;
    }
    *VT = vocabulary.size();
}

//
// コーパスの単語数を返す関数
//
int countWords(const std::string& test_file)
{
    std::ifstream test_input(test_file);
    std::string line;
    int nof_words = 0;
    while (std::getline(test_input, line))
    {
        auto words = split(line, SENTENCE_DELIMITER);
        nof_words += words.size();
    }
    return nof_words;
}