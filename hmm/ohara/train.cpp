//
// train.txt（形式は以下）からタグの並びを抽出するスクリプト
//
////////////////////// 実行例 ////////////////////////////
// $ zcat train.txt.gz | ./train > model
//
//////////////////// 入力 (train.txt) /////////////////////
// He PRP B-NP
// is VBZ B-VP
// (He is から始まる一文の他の単語のタグ等の情報)
// deficits NNS I-NP
// . . O
// (改行で文の区切り)
//
#include "common.h"

class Stat {
public:
    std::vector<std::string> id2tag, id2word;
    std::vector<std::vector<int>> transition; // tag (pre) -> tag
    std::vector<int> start_transition;
    std::vector<int> end_transition;
    std::vector<std::vector<int>> emission; // tag -> word
    std::vector<int> tag_id2count, word_id2count;
    size_t nof_sentences, nof_tokens;
    Stat() {
        id2tag = {}, id2word = {}, transition = {},
        start_transition = {}, end_transition = {}, emission = {},
        tag_id2count = {}, word_id2count = {};
        nof_sentences = 0, nof_tokens = 0;
    }
    size_t nof_tags() const { return id2tag.size(); }
    size_t nof_words() const { return id2word.size(); }
    int add_tag (std::string tag) {
        id2tag.push_back(tag);
        // extend vectors
        for (size_t i = 0; i < transition.size(); ++i)
            transition[i].push_back(0);
        for (size_t i = 0; i < emission.size(); ++i)
            emission[i].push_back(0);
        transition.push_back(std::vector<int> (nof_tags(), 0));
        start_transition.push_back(0);
        end_transition.push_back(0);
        // count
        tag_id2count.push_back(0);
        return (int) nof_tags() - 1;
    }
    int add_word (std::string word) {
        // extend vectors
        id2word.push_back(word);
        emission.push_back(std::vector<int> (nof_tags() + 1, 0));
        // count
        word_id2count.push_back(0);
        return (int) nof_words() - 1;
    }
    // debug
    void show_all () {
        std::cout << "{" << std::endl
        << "  id2tag: " << id2tag << std::endl
        << "  id2word: " << id2word << std::endl
        << "  transition: " << transition << std::endl
        << "  start_transition: " << start_transition << std::endl
        << "  end_transition: " << end_transition << std::endl
        << "  emission: " << emission << std::endl
        << "  tag_id2count: " << tag_id2count << std::endl
        << "  word_id2count: " << word_id2count << std::endl
        << "  nof_sentences: " << nof_sentences << std::endl
        << "  nof_tokens: " << nof_tokens << std::endl
        << "}" << std::endl;
    }
};

void train(Stat* stat) {
    char line[1 << 21];

    std::unordered_map<std::string, int> word2id, tag2id;
    int word_id(-1), tag_id(-1), prev_tag_id(-1);

    while (fgets(line, 1 << 21, stdin)) {
        if (line[0] == '\n') {
            ++stat->end_transition[tag_id];
            ++stat->nof_sentences;
            prev_tag_id = -1;
            continue;
        }
        // word2id
        char* mv = line;
        char* stay = mv; while (*mv != TXT_DELIMITER) ++mv;
        std::string word = std::string(stay, mv - stay);
        auto it = word2id.find(word);
        if(it != word2id.end())
            word_id = it->second;
        else {
            word_id = stat->add_word(word);
            word2id.emplace(word, word_id);
        }
        // tag2id
        ++mv;
        stay = mv; while (*mv != TXT_DELIMITER) ++mv;
        std::string tag = std::string(stay, mv - stay);
        it = tag2id.find(tag);
        if(it != tag2id.end())
            tag_id = it->second;
        else {
            tag_id = stat->add_tag(tag);
            tag2id.emplace(tag, tag_id);
        }
        // calc values by using word_id and tag_id
        ++stat->nof_tokens;
        ++stat->tag_id2count[tag_id];
        ++stat->word_id2count[word_id];
        ++stat->emission[word_id][tag_id];
        if (prev_tag_id == -1)
            ++stat->start_transition[tag_id];
        else
            ++stat->transition[tag_id][prev_tag_id];
        prev_tag_id = tag_id;
    }
}

void write_model(const Stat& stat) {
    const size_t& nof_tags = stat.nof_tags();
    const size_t& nof_words = stat.nof_words();
    // output tags
    for (size_t i = 0; i < nof_tags; ++i)
        std::fprintf(stdout, "%s\n", stat.id2tag[i].c_str());
    std::fprintf(stdout, "\n");
    // output words
    for (size_t i = 0; i < nof_words; ++i)
        std::fprintf(stdout, "%s\n", stat.id2word[i].c_str());
    std::fprintf(stdout, "\n");
    // output start_transition prob
    for (size_t i = 0; i < nof_tags; ++i)
        std::fprintf(stdout, "%g ", (1.0 * stat.start_transition[i] / stat.nof_sentences));
    std::fprintf(stdout, "\n\n");
    // output end_transition prob
    for (size_t i = 0; i < nof_tags; ++i)
        std::fprintf(stdout, "%g ",(1.0 * stat.end_transition[i] / stat.nof_sentences));
    std::fprintf(stdout, "\n\n");
    // output transition prob
    const double LAMBDA_TRANS = 1.0;
    for (size_t tag_id = 0; tag_id < nof_tags; ++tag_id) {
        for (size_t prev_tag_id = 0; prev_tag_id < nof_tags; ++prev_tag_id) {
            // naive laplace smoothing
            int numer = stat.transition[tag_id][prev_tag_id] + LAMBDA_TRANS; // 分子
            int denom = stat.tag_id2count[prev_tag_id] + stat.nof_tags() * LAMBDA_TRANS; // 分母
            std::fprintf(stdout, "%g ", (1.0 * numer / denom));
        }
        std::fprintf(stdout, "\n");
    }
    std::fprintf(stdout, "\n");
    // output emission prob
    const double LAMBDA_EMI = 1.0;
    for (size_t word_id = 0; word_id < nof_words; ++word_id) {
        for (size_t tag_id = 0; tag_id < nof_tags; ++tag_id) {
            // naive laplace smoothing
            int numer = stat.emission[word_id][tag_id] + LAMBDA_EMI; // 分子
            int denom = stat.tag_id2count[tag_id] + (nof_words * LAMBDA_EMI) + 1; // 分母
            std::fprintf(stdout, "%g ", (1.0 * numer / denom));
        }
        std::fprintf(stdout, "\n");
    }

}

int main(int argc, char** argv) {
    Stat stat;

    train(&stat);
    write_model(stat);

    std::cerr << "# sentences: " << stat.nof_sentences << std::endl
    << "# tokens: " << stat.nof_tokens << std::endl
    << "# words: " << stat.nof_words() << std::endl
    << "# tags: " << stat.nof_tags() << std::endl;

    return 0;
}