//
// train.txt（形式は以下）からタグと単語の組をカウントするスクリプト
//
////////////////////// 実行例 ////////////////////////////
// $ cat train.txt | ./txt2tagword_count | tail -10
// 77 WRB where
// 3 WRB wherever
// 20 WRB why
// 28 `` `
// 1503 `` ``
//
/////////////////////  入力 (train.txt) //////////////////////
// He PRP B-NP
// is VBZ B-VP
// (He is から始まる一文の他の単語のタグ等の情報)
// deficits NNS I-NP
// . . O
// (改行で文の区切り)
//
//////////////////// 出力例 (タグと単語の組とその出現回数) ///////////
// 2331 PRP He
// 6778 VBZ is
// ...
// 26541 . .
//
#include "common.h"

int main() {
    // タグ順の出現回数保存用map
    std::map<std::string, int> tagword_freq;

    char char_line[1 << 21];
    while (fgets(char_line, 1 << 21, stdin))
    {
        if (char_line[0] == '\n')
        {
            continue;
        }
        else
        {
            // タグと単語を抽出
            std::string line = char_line;
            size_t first_delim_pos = line.find(TXT_DELIMITER);
            size_t second_delim_pos = line.find(TXT_DELIMITER, first_delim_pos + 1);
            int tag_len = second_delim_pos - first_delim_pos - 1;
            std::string word = line.substr(0, first_delim_pos);
            std::string tag = line.substr(first_delim_pos + 1, tag_len);
            std::string tagword = tag + SENTENCE_DELIMITER + word;

            // カウント
            auto iter = tagword_freq.find(tagword);
            if (iter != tagword_freq.end())
                iter->second++;
            else
                tagword_freq.emplace(tagword, 1);
        }
    }

    // output
    auto tagword_freq_end = tagword_freq.end();
    for(auto iter = tagword_freq.begin(); iter != tagword_freq_end; iter++)
    {
        std::cout << iter->second << SENTENCE_DELIMITER << iter->first << std::endl;
    }

    return 0;
}