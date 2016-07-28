#include "pos.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <queue>
#include <functional>

using std::cout;
using std::endl;
using std::string;

using namespace nlp;

const string START = "<s>";
const string END = "</s>";
const string BREAK = "";
const string DELIMITER = " ";
const char DELIMITER_C = ' ';

double threshold = 0;
const int top_k = 2;
const int unk_threshold = 3;

POS::POS(const int n) : N(n)
{
	max_sentence_size = 0;
	dp = nullptr;
	bp = nullptr;
	transition_table_file = "transition_table-" + std::to_string(N) + "-gram.save";
	observation_table_file = "observation_table.save";
	unk_table_file = "unk_table.save";
	vocab_file = "vocab.save";
	suffix_file = "suffixes.data";

	load_suffix(suffix_file);
}

POS::~POS()
{
	int tagger_size = taggers.size();
	for (int i = 0; i < tagger_size; i++)
	{
		delete[] dp[i];
		delete[] bp[i];
	}
	delete[] dp;		
	delete[] bp;
}

void POS::train(const string &train_file)
{
	const int KEY_SIZE = N - 1;
	int sentence_count = 0, token_count = 0;
	StrVec vkey_start(KEY_SIZE, START);
	StrVec vkey = vkey_start;
	std::istream *fin = nullptr;

	if (train_file == "")
		fin = &std::cin;
	else
	{
		std::ifstream *_fin = new std::ifstream(train_file);
		if (!_fin->is_open()) {
			std::cerr << "ERROR: No such file (" << train_file << ")" << std::endl;
			std::exit(EXIT_FAILURE);
		}
		fin = _fin;
	}

	string line = "";
	while (std::getline(*fin, line))
	{
		if (line == BREAK)
		{
			sentence_count++;
			transition_table.insert(hash(vkey), END);
			if (KEY_SIZE > 0)
				vkey = vkey_start;
		}
		else
		{
			token_count++;
			std::istringstream iss(line);
			string token;
			std::getline(iss, token, DELIMITER_C);
			string tagger;
			std::getline(iss, tagger, DELIMITER_C);

			observation_table.insert(tagger, token);
			transition_table.insert(hash(vkey), tagger);

			vocab.insert(token);

			if (KEY_SIZE > 0)
			{
				vkey.erase(vkey.begin());
				vkey.push_back(tagger);
			}
		}
	}

	for (auto key_value : observation_table.table)
	{
		TableKey tagger = key_value.first;
		TableValue value = key_value.second;
		for (auto counter_pair : value->counter)
		{
			string token = counter_pair.first;
			long count = counter_pair.second;
			if (count < unk_threshold)
			{
				string unk_type = classify_unk(token);
				unk_table.insert(tagger, unk_type);
			}
		}
	}

	if (train_file != "")
	{
		((std::ifstream*)fin)->close();
		delete fin;
	}

	save_vocab(vocab_file);
	observation_table.serialize(observation_table_file);
	transition_table.serialize(transition_table_file);
	unk_table.serialize(unk_table_file);

	cout << "#sentences: " << sentence_count << endl
	     << "#tokens:    " << token_count << endl
	     << "#words:     " << vocab.size() << endl
	     << "#tags:      " << observation_table.keys().size() << endl;
}

void POS::test(const string &test_file)
{
	load_vocab(vocab_file);
	cout << "loading observation table..." << endl;
	observation_table.deserialize(observation_table_file);
	cout << "loading transition table..." << endl;
	transition_table.deserialize(transition_table_file);
	cout << "loading unknown words table..." << endl;
	unk_table.deserialize(unk_table_file);

	taggers = observation_table.keys();

	cout << "reading test data..." << endl;
	read_test_data(test_file);

	cout << "tagging..." << endl;

	create_dynamic_space();

	size_t test_size = test_taggers_list.size();
	//hit:0, hit_unk:1, total:2, total_unk:3
	long count[4] = {0};

	for (size_t i = 0; i < test_size; i++)
	{
		StrVec &tokens = test_tokens_list[i];
		StrVec &test_taggers = test_taggers_list[i];
		StrVec predict_taggers = viterbi(tokens);
		hit_count(test_taggers, predict_taggers, tokens, count);
	}

	double accuracy = (double)count[0] / (double)count[2];
	double accuracy_kn = (double)(count[0]-count[1]) / (double)(count[2]-count[3]);
	double accuracy_ukn = (double)count[1] / (double)count[3];

	cout << "accuracy: " << accuracy << endl;
	cout << "accuracy (known): " << accuracy_kn << endl;
	cout << "accuracy (unknown): " << accuracy_ukn << endl;
}

//hit:0, hit_unk:1, total:2, total_unk:3
void POS::hit_count(const StrVec &test_taggers, const StrVec &predict_taggers, const StrVec &tokens, long *count)
{
	size_t size = test_taggers.size();
	count[2] += size;
	for (size_t i = 0; i < size; i++)
	{
		if (vocab.find(tokens[i]) != vocab.end())
		{
			if (test_taggers[i] == predict_taggers[i])
				count[0]++;
		}
		else
		{
			if (test_taggers[i] == predict_taggers[i])
			{
				count[0]++;
				count[1]++;
			}
			count[3]++;
		}		
	}
}

void POS::read_test_data(const string &test_file)
{
	const int KEY_SIZE = N - 1;
	StrVec vkey_start(KEY_SIZE, START);
	StrVec vkey = vkey_start;

	std::istream *fin = nullptr;
	if (test_file == "")
		fin = &std::cin;
	else
	{
		std::ifstream *_fin = new std::ifstream(test_file);
		if (!_fin->is_open()) {
			std::cerr << "ERROR: No such file (" << test_file << ")" << std::endl;
			std::exit(EXIT_FAILURE);
		}
		fin = _fin;
	}

	string line;
	StrVec _tokens, _taggers;
	int sentence_len = 0;
	while (std::getline(*fin, line))
	{
		if (line == BREAK)
		{
			if (sentence_len > max_sentence_size)
				max_sentence_size = sentence_len;
			sentence_len = 0;

			test_tokens_list.push_back(_tokens);
			test_taggers_list.push_back(_taggers);
			_tokens.clear();
			_taggers.clear();
		}
		else
		{
			sentence_len++;
			std::istringstream iss(line);
			string str;
			std::getline(iss, str, DELIMITER_C);
			_tokens.push_back(str);
			std::getline(iss, str, DELIMITER_C);
			_taggers.push_back(str);
		}
	}

	if (test_file != "")
	{
		((std::ifstream*)fin)->close();
		delete fin;
	}
}

void POS::create_dynamic_space()
{
	int tagger_size = taggers.size();
	dp = new double *[tagger_size];
	for (int i = 0; i < tagger_size; i++)
		dp[i] = new double[max_sentence_size];

	bp = new int *[tagger_size];
	for (int i = 0; i < tagger_size; i++)
		bp[i] = new int[max_sentence_size];
}

void POS::forward(const StrVec &sentence)
{
	double prob = dp_algo(sentence, "forward");
	cout << "likelihood: " << prob << endl;
}

StrVec POS::viterbi(const StrVec &sentence)
{
	double prob = dp_algo(sentence, "viterbi");

	int backpointer = bp[0][0];
	StrVec tagged {taggers[backpointer]};

	int sentence_size = sentence.size();
	for (int i = sentence_size-1; i > 0; i--)
	{
		backpointer = bp[backpointer][i];
		tagged.insert(tagged.begin(), taggers[backpointer]);
	}

	return tagged;
}

double POS::dp_algo(const StrVec &sentence, const string &type)
{
	const int tagger_size = taggers.size();
	const int sentence_size = sentence.size();

	for (int h = 0; h < sentence_size; h++)
	{
		string token;
		bool unk = false;
		if (vocab.find(sentence[h]) != vocab.end())
			token = sentence[h];
		else
		{
			token = classify_unk(sentence[h]);
			unk = true;
		}
			

		std::priority_queue<double, std::vector<double>, std::greater<double> > min_heap;
		for (int v = 0; v < tagger_size; v++)
		{
			double pt = path_prob(sentence, h, v, type);
			double po = observation_prob(taggers[v], token, unk);

			dp[v][h] = pt * po;
			if (min_heap.size() < top_k)
				min_heap.push(dp[v][h]);
			else if(dp[v][h] > min_heap.top())
			{
				min_heap.pop();
				min_heap.push(dp[v][h]);
			}
		}
		threshold = min_heap.top();
	}

	return path_prob(sentence, sentence_size, 0, type);
}

double POS::path_prob(const StrVec &sentence, const int h, const int v, const string &type)
{
	int tagger_size = taggers.size();
	int sentence_size = sentence.size();
	string tagger = h == sentence_size ? END : taggers[v];

	if (h == 0)
		return transition_prob(START, tagger);

	if (type == "forward")
	{
		double sum_pt = 0;
		for (int i = 0; i < tagger_size; i++)
		{
			double p_dp = dp[i][h-1];
			if (p_dp == 0) continue;

			sum_pt += p_dp * transition_prob(taggers[i], tagger);
		}
		return sum_pt;
	}

	double max_pt = 0;
	int backpointer = 0;
	for (int i = 0; i < tagger_size; i++)
	{
		double p_dp = dp[i][h-1];
		if (p_dp == 0 || p_dp < threshold) continue;

		double pt = p_dp * transition_prob(taggers[i], tagger);

		if (pt > max_pt)
		{
			max_pt = pt;
			backpointer = i;
		}
	}

	if (h < sentence_size)
		bp[v][h] = backpointer;
	else
		bp[0][0] = backpointer;

	return max_pt;
}

double POS::transition_prob(const string &cond, const string &tagger)
{
	TableMap &table = transition_table.table;
	const auto &key_value = table.find(cond);
	if (key_value == table.end())
		return 0;

	CounterMap &counter = key_value->second->counter;
	const auto &counter_pair = counter.find(tagger);
	if (counter_pair == counter.end())
		return 0;

	return (double)counter_pair->second / (double)key_value->second->count;
}

double POS::observation_prob(const string &tagger, const string &token, bool unk)
{
	TableMap &table = unk ? unk_table.table : observation_table.table;
	auto key_value = table.find(tagger);
	if (key_value == table.end())
		return 0;

	CounterMap &counter = key_value->second->counter;
	long total_count = key_value->second->count;
	auto counter_pair = counter.find(token);
	if (counter_pair == counter.end())
		return 0;

	long count = counter_pair->second;
	return (double)count / (double)total_count;
}

TableKey POS::hash(const StrVec& v)
{
	std::stringstream ss;
	string str_key;
	std::copy(v.begin(), v.end(), std::ostream_iterator<string>(ss, DELIMITER.c_str()));
	str_key = ss.str();

	//str_key equals 0 in unigram.
	if (str_key.size() > 0)
		str_key.pop_back();
	return str_key;
}

string POS::classify_unk(const string &token)
{
	if (token.find('-') != string::npos)
		return "F_HYP";
	if (token[0] >= 'A' && token[0] <= 'Z')
		return "F_CAP";
	if (token[0] >= '0' && token[0] <= '9')
		return "F_NUM";
	for (auto it : suffix_map)
	{
		string suffix = it.first;
		string unk_type = it.second;

		if (token.size() >= suffix.size() &&
			token.compare(token.size()-suffix.size(), suffix.size(), suffix) == 0)
			return unk_type;
	}
	return "F_OTHER";
}

void POS::load_suffix(const string &file_name)
{
	std::ifstream fin(file_name);
	if (!fin.is_open())
	{
		std::cerr << "ERROR: cannot open suffix file." << std::endl;
		std::exit(EXIT_FAILURE);
	}

	string line;
	while (std::getline(fin, line))
	{
		std::istringstream iss(line);
		string suffix;
		std::getline(iss, suffix, DELIMITER_C);
		string unk_type;
		std::getline(iss, unk_type, DELIMITER_C);
		suffix_map[suffix] = unk_type;
	}
	fin.close();
}

void POS::save_vocab(const string &file_name)
{
	std::ofstream fout(file_name);
	for (const auto& elem: vocab)
    	fout << elem << endl;

	fout.close();
}

void POS::load_vocab(const string &file_name)
{
	std::ifstream fin(file_name);
	if (!fin.is_open())
	{
		std::cerr << "ERROR: cannot open vocab file." << std::endl;
		std::exit(EXIT_FAILURE);
	}

	string line;
	while (std::getline(fin, line))
	{
		vocab.insert(line);
	}

	fin.close();
}

Table::Table() {}

Table::Table(const string &serialized_file)
{
	deserialize(serialized_file);
}

Table::~Table()
{
	for (auto value = table.begin(); value != table.end(); value++)
		delete value->second;
	table.clear();
}

void Table::insert(const TableKey key, const string target)
{
	auto key_value = table.find(key);
	if (key_value == table.end())
		table[key] = new Counter(target);
	else
		key_value->second->insert(target);
}

void Table::insert(const TableKey key, const string target, const long total_count, const long count)
{
	auto key_value = table.find(key);
	if (key_value == table.end())
		table[key] = new Counter(target, total_count, count);
	else
		key_value->second->insert(target, count);
}

StrVec Table::keys()
{
	StrVec ks;
	for (auto key_value : table)
		ks.push_back(key_value.first);
	return ks;
}

void Table::serialize(const string &file_name)
{
	std::ofstream fout(file_name);
	//save the total_count in the first line in the ngram file
	for (auto key_value : table)
	{
		TableKey key = key_value.first;
		TableValue value = key_value.second;
		long total_count = value->count;
		for (auto counter_pair : value->counter)
		{
			string target = counter_pair.first;
			long count = counter_pair.second;
			fout << count << DELIMITER << total_count << DELIMITER << target << DELIMITER << key << endl;
		}
	}
	fout.close();
}

void Table::deserialize(const string &file_name)
{
	std::ifstream fin(file_name);
	if (!fin.is_open())
	{
		std::cerr << "ERROR: cannot open serialized file." << std::endl;
		std::exit(EXIT_FAILURE);
	}

	string line;
	while (std::getline(fin, line))
	{
		std::istringstream iss(line);
		string str;
		std::getline(iss, str, DELIMITER_C);
		long count = std::stol(str);
		std::getline(iss, str, DELIMITER_C);
		long total_count = std::stol(str);

		string key, target;
		std::getline(iss, target, DELIMITER_C);
		std::getline(iss, key);

		insert(key, target, total_count, count);
	}
	fin.close();
}

Counter::Counter(const string target)
{
	counter[target] = 1;
	count = 1;
}

Counter::Counter(const string target, const long total_count, const long target_count)
{
	counter[target] = target_count;
	count = total_count;
}

void Counter::insert(const string target)
{
	auto target_pair = counter.find(target);
	if (target_pair == counter.end())
	{
		counter[target] = 1;
		count++;
	}
	else
	{
		target_pair->second++;
		count++;
	}
}

void Counter::insert(const string target, const long target_count)
{
	counter[target] = target_count;
}