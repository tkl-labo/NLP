//
// 単語間がスペースで、文が改行で区切られた文から
// ngramをカウントするスクリプト
//
#include <map>
#include "common.h"

// global variables
int N;
// 一時的にngramを格納するvector。新しい単語が来るたびに更新
std::vector<std::string> tmp_ngram;
// ngramのvectorをkeyに、その出現関数をvalueとするmap
std::map<std::vector<std::string>, int> ngram_freq;

// functions
void initializeTmpNgram();
void updateNgramFreq(std::string &word);
void updateTmpNgram(std::string &word);


int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " sentence_file N" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::string sentence_file = argv[1];
    N = std::atoi(argv[2]);

    // read sentence file
    std::ifstream input(sentence_file);
    if (!input) {
        std::cerr << "cannot find sentence file." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // loop
    std::string line;
    while (std::getline(input, line))
    {
        auto words = split(line, SENTENCE_DELIMITER);
        initializeTmpNgram();
        if (N == 1)
            updateNgramFreq(NGRAM_START_SYMBOL);
        for (std::string word : words)
            updateNgramFreq(word);
    }

    // output
    auto ngram_freq_end = ngram_freq.end();
    for(auto iter = ngram_freq.begin(); iter != ngram_freq_end; iter++)
    {
        std::cout << iter->second;
        // ngram vectorでのloop
        for (std::string ngram_word : iter->first)
            std::cout << SENTENCE_DELIMITER << ngram_word;
        std::cout << std::endl;
    }
    return 0;
}


//
// 変数tmp_ngramを初期化
//
void initializeTmpNgram()
{
    tmp_ngram.clear();
    for (int i=0; i < N-1; i++)
        updateTmpNgram(NGRAM_START_SYMBOL);
}


//
// 変数ngram_freqを更新
// ngramのvectorをkeyとし、出現回数を加算
//
void updateNgramFreq(std::string &word)
{
    updateTmpNgram(word);
    if(tmp_ngram.size() == N)
    {
        auto iter = ngram_freq.find(tmp_ngram);
        if (iter != ngram_freq.end())
            iter->second++;
        else
            ngram_freq.emplace(tmp_ngram, 1);
    }
}


//
// 変数tmp_ngramを更新する
// tmp_ngramにwordを保存していき、大きさは常にNを保つ
//
void updateTmpNgram(std::string &word)
{
    if(tmp_ngram.size() != N)
        tmp_ngram.push_back(word);
    else
    {
        // OPTIMIZE: eraseは削除された要素以降のデータがひとつずつ前に移動されるので遅そう。
        tmp_ngram.erase(tmp_ngram.begin());
        tmp_ngram.push_back(word);
    }
}
