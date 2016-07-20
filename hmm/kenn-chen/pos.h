#pragma once
#include "util.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace nlp
{
    class Counter;
    using TableKey = std::string;
    using TableValue = Counter*;
    using TableMap = std::unordered_map<TableKey, TableValue>;
    using CounterMap = std::unordered_map<std::string, long>;
    using StrVec = std::vector<std::string>;
    
    class Counter
    {
        public:
        long count;
        CounterMap counter;
        
        Counter(const std::string key);
        Counter(const std::string key, const long total_count, const long target_count);
        void insert(const std::string target);
        void insert(const std::string target, const long target_count);
    };
    
    class Table
    {
        public:
        TableMap table;
        
        Table();
        Table(std::string serialized_file);
        ~Table();
        void insert(const TableKey key, const std::string target);
        void insert(const TableKey key, const std::string target, const long total_count, const long count);
        void serialize(std::string file_name);
        void deserialize(std::string file_name);
        StrVec keys();
    };
    
    class POS
    {
        private:
        const int N;
        StrVec taggers;
        int max_sentence_size;
        double **dp;
        int **bp;
        std::unordered_set<std::string> vocab;
        std::string transition_table_file;
        std::string observation_table_file;
        std::string vocab_file;
        std::vector<StrVec> test_tokens_list;
        std::vector<StrVec> test_taggers_list;
        Table transition_table;
        Table observation_table;
        
        public:
        POS(const int n);
        ~POS();
        void train(const std::string train_file);
        void test(const std::string test_file);

        private:
        void read_test_data(std::string test_file);
        void create_dynamic_space();
        long correct_count(StrVec sv1, StrVec sv2, StrVec &tokens, long &coc_kn, long &total_count_kn);
        TableKey hash(const StrVec& v);
        void forward(StrVec &sentence);
        StrVec viterbi(StrVec &sentence);
        double bi_dp_algo(StrVec &sentence, const std::string type);
        //pos_pair dp_algo(double **dp, Number **bp, StrVec &taggers, StrVec &sentence, const std::string type);
        double p_transition(std::string cond, std::string tagger);
        double p_transition(StrVec cond, std::string tagger);
        double bigram_p_transition(const StrVec &sentence, const int h, const int v, const std::string type);
        //double n_gram_p_transition(double **dp, Number **bp, const StrVec &taggers, const StrVec &sentence, const int h, const int v, const std::string type);
        double p_observation(std::string tagger, std::string token);
        void save_vocab(std::string file_name);
        void load_vocab(std::string file_name);
    };
}