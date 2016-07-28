#pragma once
#include <vector>
#include <string>
#include <map>
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

struct suffix_cmp
{
	bool operator()(const std::string& a, const std::string& b) const
	{
		if (a.length() != b.length())
			return a.length() > b.length();
		else
			return a < b;
	}
}; 

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
	std::string unk_table_file;
	std::string suffix_file;
	std::string vocab_file;
	std::vector<StrVec> test_tokens_list;
	std::vector<StrVec> test_taggers_list;
	Table transition_table;
	Table observation_table;
	Table unk_table;
	std::map<std::string, std::string, suffix_cmp> suffix_map;

public:
	POS(const int n);
	~POS();
	void train(const std::string train_file);
	void test(const std::string test_file);

private:
	void forward(StrVec &sentence);
	StrVec viterbi(StrVec &sentence);
	double dp_algo(StrVec &sentence, const std::string type);
	double transition_prob(std::string cond, std::string tagger);
	double observation_prob(std::string tagger, std::string token, bool unk);
	double path_prob(const StrVec &sentence, const int h, const int v, const std::string type);
	std::string classify_unk(std::string token);
	void read_test_data(std::string test_file);
	void load_suffix(std::string file_name);
	void save_vocab(std::string file_name);
	void load_vocab(std::string file_name);
	TableKey hash(const StrVec& v);
	void hit_count(StrVec &test_taggers, StrVec &predict_taggers, StrVec &tokens, long *count);
	void create_dynamic_space();
};
}