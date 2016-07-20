//
// 一文のタグの並びから指定したn個のタグ順を取得するスクリプト
//
////////////////////// 実行例 ////////////////////////////
// $ cat train.txt | ./txt2tagseq | ./tagseq2tagseq_count 1 | head -5
// 36 #
// 1750 $
// 1493 ''
// 274 (
// 281 )
// $ cat train.txt | ./txt2tagseq | ./tagseq2tagseq_count 2 | head -5
// 36 # CD
// 1724 $ CD
// 7 $ JJ
// 16 $ NN
// 1 $ NNP
//
////////////////////////// 入力 //////////////////////////
// PRP VBZ ... NNS .
// （次の一文のタグ並び）
//
///////////////// 出力例 (タグ順とその出現回数) ///////////////
// 353 PRP VBZ
// ...
// 1000 NNS .
//
#include "common.h"

size_t find_nth_pos(const std::string& str, size_t start_pos, const char& needle, size_t N);

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " N" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    // N個分のタグをタグ順として切り出していく
    int N = std::atoi(argv[1]);

    // タグ順の出現回数保存用map
    std::map<std::string, int> tagseq_freq;

    char char_line[1 << 21];
    // 一文ずつループ
    while (fgets(char_line, 1 << 21, stdin))
    {
        // 文頭シンボルと文末シンボルを追加
        std::string line = TAG_START_SYMBOL + SENTENCE_DELIMITER + char_line;
        line = line.replace(line.find('\n'), 1, TAG_END_SYMBOL);

        // タグ順をカウントする
        size_t found_pos = 0;
        // 単語間のdelimを手がかりに(findで見つけ)ループ
        while (found_pos != std::string::npos)
        {
            // タグ順を抽出
            size_t first_char_pos = (found_pos == 0) ? found_pos : found_pos + 1;
            size_t tagseq_end_pos = find_nth_pos(line, first_char_pos, SENTENCE_DELIMITER, N);
            int tagseq_len = tagseq_end_pos - first_char_pos;
            std::string tagseq = line.substr(first_char_pos, tagseq_len);

            // カウント
            auto iter = tagseq_freq.find(tagseq);
            if (iter != tagseq_freq.end())
                iter->second++;
            else
                tagseq_freq.emplace(tagseq, 1);

            // tagseqが一番最後のタグまで含んだので、この文の解析を終了する
            if (tagseq_end_pos == std::string::npos) break;

            // 次のdelimを取得する
            found_pos = line.find(SENTENCE_DELIMITER, first_char_pos);
        }
    }

    // output
    auto tagseq_freq_end = tagseq_freq.end();
    for(auto iter = tagseq_freq.begin(); iter != tagseq_freq_end; iter++)
    {
        std::cout << iter->second << SENTENCE_DELIMITER << iter->first << std::endl;
    }

    return 0;
}

//
// 対象文字列の開始位置から探索文字をn回見つけ、そのn番目の位置を返す関数
// 探索文字がn回存在しない場合存在しない場合，std::string::nposを返す
//
// @param std::string str: 探索対象文字列
// @param size_t start_pos: 探索開始位置
// @param char needle: 探索文字
// @param size_t nth: n回見つける
// @return 発見した位置
//
size_t find_nth_pos(const std::string& str, size_t start_pos, const char& needle, size_t nth)
{
    size_t found_pos = str.find(needle, start_pos);
    if (nth == 1 || found_pos == std::string::npos)  return found_pos;
    return find_nth_pos(str, found_pos + 1, needle, nth - 1);
}