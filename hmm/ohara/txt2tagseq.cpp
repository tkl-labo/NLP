//
// train.txt（形式は以下）からタグの並びを抽出するスクリプト
//
////////////////////// 実行例 ////////////////////////////
// $ cat train.txt | ./txt2tagseq | head -5
// NN IN DT NN VBZ RB VBN TO VB DT JJ NN IN NN NNS IN NNP , JJ IN NN NN , VB TO VB DT JJ NN IN NNP CC NNP POS JJ NNS .
// NNP IN DT NNP NNP NNP POS VBN NN TO DT NN JJ NN VBZ VBN TO VB DT NN IN NN IN DT JJ NN .
// CC NNS VBP VBG NN IN NN VBZ VBN VBN IN DT NN POS NN TO VB DT JJ NN NNS IN PRP$ NNP NNP NN JJ NNP .
// DT VBZ VBN DT NN IN DT NN VBG VBN TO VB NN NNS TO CD NN IN PRP$ JJ CD NN NN TO VB DT NN , NNS CC JJ NN NN NNS VBP .
// `` DT NNS IN NN IN DT JJ NN NN VBP RB RB IN DT JJ NN , '' VBD NNP NNP , JJ NNP NN IN NNP NNP NNP .
//
//////////////////// 入力 (train.txt) /////////////////////
// He PRP B-NP
// is VBZ B-VP
// (He is から始まる一文の他の単語のタグ等の情報)
// deficits NNS I-NP
// . . O
// (改行で文の区切り)
//
//////////////// 出力例 (一文のタグの並びを一列に) ///////////////
// PRP VBZ ... NNS .
//
//
#include "common.h"

int main() {
    char char_line[1 << 21];
    while (fgets(char_line, 1 << 21, stdin))
    {
        if (char_line[0] == '\n')
        {
            std::cout << std::endl;
        }
        else
        {
            std::string line = char_line;
            size_t first_delim_pos = line.find(TXT_DELIMITER);
            size_t second_delim_pos = line.find(TXT_DELIMITER, first_delim_pos + 1);
            int tag_len = second_delim_pos - first_delim_pos - 1;
            std::string tag = line.substr(first_delim_pos + 1, tag_len);
            std::cout << tag << SENTENCE_DELIMITER;
        }
    }
    return 0;
}