//
// Created by OharaKohei on 2016/06/28.
//
#include "ngram.h"

std::string NGRAM_START_SYMBOL = "<s>";
std::string NGRAM_END_SYNBOL = "</s>";
std::string SENTENCE_END_SYMBOL = "EOS";

NGram::NGram(const int N)
{
    N_ = N;
    nof_corpus_words_ = 0;
}

void NGram::train(const std::string &train_data)
{
    std::cout << "training..." << std::endl;
    std::ifstream input(train_data);
    if (!input) {
        std::cerr << "cannot found training data file!" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    countNgram_(input);
    input.close();
}

//
// perplexityを算出するためにパラメータを更新
// nof_corpus_wordsを計算する必要がある
//
void NGram::test(const std::string &test_data)
{
    std::cout << "testing..." << std::endl;
    std::ifstream input(test_data);
    if (!input) {
        std::cerr << "cannot found test data file!" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    test_(input);
    input.close();
}

//
// パラメータ更新実行関数
//
void NGram::test_(std::istream &stream)
{
    std::string line;
    std::unordered_set<std::string> uniqueWords = {};
    while (std::getline(stream, line))
    {
        std::string word = getSurface_(line);
        uniqueWords.emplace(word);
    }
    // <s>の分で1を加えた数がcorpusのuniqueな文字数
    nof_corpus_words_ = uniqueWords.size() + 1;
}

//
// test()を呼びパラメータが更新できたら、
// perplexityを計算
//
double NGram::calcPerplexity(const std::string &test_data)
{
    std::cout << "calculating perplexity.." << std::endl;
    std::ifstream input(test_data);
    if (!input) {
        std::cerr << "cannot found test data file!" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    double pp = calcPerplexity_(input);
    input.close();
    return pp;
}

//
// perplexity計算の実行メソッド
//
double NGram::calcPerplexity_(std::istream &stream)
{
    double perplexity = 1.0;
    initializeTmpNgram_();
    std::string line;
    while (std::getline(stream, line))
    {
        std::string word = getSurface_(line);
        if (word == SENTENCE_END_SYMBOL)
        {
            word = NGRAM_END_SYNBOL;
        }
        updateTmpNgram_(word);
        const double p = probability_(tmp_ngram_);
        const double pp = std::pow(1.0 / p, 1.0 / nof_corpus_words_);
        perplexity *= pp;
        if (word == SENTENCE_END_SYMBOL)
        {
            initializeTmpNgram_();
        }
    }
    return perplexity;
}

void NGram::showProbabilities()
{
    auto end = ngram_freq_.end();
    for(auto iter = ngram_freq_.begin(); iter != end; iter++)
    {
        double p = probability_(iter->first);

        std::vector<std::string> v(iter->first.begin(), iter->first.end()-1);
        std::cout << "P( " << iter->first << " | "
            << v << " ) = " << p << std::endl;
    }
}

//
// ngramがそれぞれどれだけ出現するかカウントする
//
void NGram::countNgram_(std::istream &stream)
{
    std::string line;
    initializeTmpNgram_();
    std::unordered_set<std::string> uniqueWords = {};
    while (std::getline(stream, line))
    {
        std::string word = getSurface_(line);
        if (word == SENTENCE_END_SYMBOL)
        {
            word = NGRAM_END_SYNBOL;
        }
        updateNgramFreq_(word);
        uniqueWords.emplace(word);
    }
    // <s>の分で1を加えた数がcorpusのuniqueな文字数。</s>はEOSとして出現するので+2ではない。
    nof_corpus_words_ = uniqueWords.size() + 1;
    // calc n-1gram histogram
    createN1gramFreq_();
}

//
// 文頭で実行。NGRAM_START_SYMBOLをN-1だけ挿入する
//
void NGram::initializeTmpNgram_()
{
    tmp_ngram_.clear();
    for (int i=0; i < N_; i++)
    {
        updateTmpNgram_(NGRAM_START_SYMBOL);
    }
}

//
// 表層系（単語そのもの）を行から取得するメソッド
// 例)
// line = "あけおめ	名詞,普通名詞,*,*"
// return "あけおめ"
//
std::string NGram::getSurface_(std::string &line)
{
    std::string delimiter = "\t";
    std::string surface = line.substr(0, line.find(delimiter));
    return surface;
}

//
// ngramのhashを作るメソッド
// ngramベクトルをkeyにしてその頻度を計算する
//
void NGram::updateNgramFreq_(std::string &word)
{
    updateTmpNgram_(word);
    if(tmp_ngram_.size() == N_)
    {
        auto iter = ngram_freq_.find(tmp_ngram_);
        if (iter != ngram_freq_.end())
        {
            iter->second++;
        }
        else
        {
            ngram_freq_.emplace(tmp_ngram_, 1);
        }
    }
    if (word == NGRAM_END_SYNBOL)
    {
        initializeTmpNgram_();
    }

}

//
// tmp_ngramにwordを保存していく
// 大きさは常にNを保つ
//
void NGram::updateTmpNgram_(std::string &word)
{
    if(tmp_ngram_.size() != N_)
    {
        tmp_ngram_.push_back(word);
    }
    else
    {
        // OPTIMIZE: eraseは削除された要素以降のデータがひとつずつ前に移動されるので遅そう。
        tmp_ngram_.erase(tmp_ngram_.begin());
        tmp_ngram_.push_back(word);
    }
}


//
// ngramの頻度mapから
// n-1gramの頻度mapを生成
//
void NGram::createN1gramFreq_()
{
    auto end = ngram_freq_.end();
    for(auto iter = ngram_freq_.begin(); iter != end; iter++)
    {
        std::vector<std::string> tmp_n_1gram(iter->first.begin(), iter->first.end()-1);
        n_1gram_freq_[tmp_n_1gram] += iter->second;
    }
}

double NGram::probability_(const std::vector<std::string> &ngram)
{
    // laplace smoothing
    // 分子, 分母
    int numer, denom;
    // 分子の計算
    auto ngram_iter = ngram_freq_.find(ngram);
    if (ngram_iter != ngram_freq_.end())
        numer = ngram_iter->second + 1;
    else
        numer = 1;
    // 分母の計算
    std::vector<std::string> v(ngram.begin(), ngram.end()-1);
    auto iter = n_1gram_freq_.find(v);
    if (iter != n_1gram_freq_.end())
        denom = iter->second + nof_corpus_words_;
    else
        denom = nof_corpus_words_;

    return numer / (double) denom;
}
