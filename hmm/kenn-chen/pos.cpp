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
const string UNK = "<unk>";
const string BREAK = "";
const string DELIMITER = " ";
const char DELIMITER_C = ' ';

double threshold = 0;
const int top_k = 2;

POS::POS(const int n) : N(n)
{
	max_sentence_size = 0;
	dp = nullptr;
	bp = nullptr;
	vocab = {END, UNK};
	transition_table_file = "transition_table-" + std::to_string(N) + "-gram.save";
	observation_table_file = "observation_table.save";
	vocab_file = "vocab.save";
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

void POS::train(const string train_file)
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

	if (train_file != "")
	{
		((std::ifstream*)fin)->close();
		delete fin;
	}

	save_vocab(vocab_file);
	observation_table.serialize(observation_table_file);
	transition_table.serialize(transition_table_file);

	cout << "#sentences: " << sentence_count << endl
	     << "#tokens:    " << token_count << endl
	     << "#words:     " << vocab.size() - 2 << endl
	     << "#tags:      " << observation_table.keys().size() << endl;
}

void POS::test(const string test_file)
{
	load_vocab(vocab_file);
	cout << "loading observation table..." << endl;
	observation_table.deserialize(observation_table_file);
	cout << "loading transition table..." << endl;
	transition_table.deserialize(transition_table_file);

	taggers = observation_table.keys();

	cout << "reading test data..." << endl;
	read_test_data(test_file);

	cout << "tagging..." << endl;

	create_dynamic_space();

	int test_size = test_taggers_list.size();
	long coc = 0, total_count = 0;
	long coc_kn = 0, total_count_kn = 0;
	for (int i = 0; i < test_size; i++)
	{
		StrVec predict_taggers = viterbi(test_tokens_list[i]);
		total_count += predict_taggers.size();
		coc += correct_count(test_taggers_list[i], predict_taggers, test_tokens_list[i], coc_kn, total_count_kn);
	}

	double accuracy = (double)coc / (double)total_count;
	double accuracy_kn = (double)coc_kn / (double)total_count_kn;
	double accuracy_ukn = (double)(coc - coc_kn) / (double)(total_count - total_count_kn);

	cout << "accuracy: " << accuracy << endl;
	cout << "accuracy for known words: " << accuracy_kn << endl;
	cout << "accuracy for unknown words: " << accuracy_ukn << endl;
}

long POS::correct_count(StrVec sv1, StrVec sv2, StrVec &tokens, long &coc_kn, long &total_count_kn)
{
	int size = sv1.size();
	long count = 0;
	for (int i = 0; i < size; i++)
	{
		if (vocab.find(tokens[i]) != vocab.end())
		{
			if (sv1[i] == sv2[i])
			{
				count++;
				coc_kn++;
			}
			total_count_kn++;
		}
		else if (sv1[i] == sv2[i])
		{
			count++;
		}
		
	}
	return count;
}

void POS::read_test_data(string test_file)
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

void POS::forward(StrVec &sentence)
{
	double prob = bi_dp_algo(sentence, "forward");
	cout << "likelihood: " << prob << endl;
}

StrVec POS::viterbi(StrVec &sentence)
{
	double prob = bi_dp_algo(sentence, "viterbi");

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

double POS::bi_dp_algo(StrVec &sentence, const string type)
{
	const int tagger_size = taggers.size();
	const int sentence_size = sentence.size();

	for (int h = 0; h < sentence_size; h++)
	{
		std::priority_queue<double, std::vector<double>, std::greater<double> > min_heap;
		for (int v = 0; v < tagger_size; v++)
		{
			double pt = bigram_p_transition(sentence, h, v, type);
			double po = p_observation(taggers[v], sentence[h]);
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

	return bigram_p_transition(sentence, sentence_size, 0, type);
}

double POS::bigram_p_transition(const StrVec &sentence, const int h, const int v, const string type)
{
	int tagger_size = taggers.size();
	int sentence_size = sentence.size();
	string tagger = h == sentence_size ? END : taggers[v];

	if (h == 0)
		return p_transition(START, tagger);

	if (type == "forward")
	{
		double sum_pt = 0;
		for (int i = 0; i < tagger_size; i++)
		{
			double p_dp = dp[i][h-1];
			if (p_dp == 0) continue;

			sum_pt += p_dp * p_transition(taggers[i], tagger);
		}
		return sum_pt;
	}

	double max_pt = 0;
	int backpointer = 0;
	for (int i = 0; i < tagger_size; i++)
	{
		double p_dp = dp[i][h-1];
		if (p_dp < threshold) continue;

		double pt = p_dp * p_transition(taggers[i], tagger);

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

double POS::p_transition(string cond, string tagger)
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

double POS::p_transition(StrVec cond, string tagger)
{
	return p_transition(hash(cond), tagger);
}

double POS::p_observation(string tagger, string token)
{
	TableMap &table = observation_table.table;
	auto key_value = table.find(tagger);
	if (key_value == table.end())
		return 1 / (double)vocab.size();

	CounterMap &counter = key_value->second->counter;
	long total_count = key_value->second->count;
	auto counter_pair = counter.find(token);
	if (counter_pair == counter.end())
		return 1 / ((double)(total_count + vocab.size()));

	long count = counter_pair->second;
	return (double)(count + 1) / ((double)(total_count + vocab.size()));
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

void POS::save_vocab(string file_name)
{
	std::ofstream fout(file_name);
	for (const auto& elem: vocab)
    	fout << elem << endl;

	fout.close();
}

void POS::load_vocab(string file_name)
{
	std::ifstream fin(file_name);
	if (!fin.is_open())
	{
		std::cerr << "ERROR: cannot open parameter file." << std::endl;
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

Table::Table(string serialized_file)
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

void Table::serialize(string file_name)
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

void Table::deserialize(string file_name)
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



/*pos_pair POS::dp_algo(double **dp, Number **bp, StrVec &taggers, StrVec &sentence, const string type)
{
	pos_pair result;
	const int tagger_size = taggers.size();
	const int sentence_size = sentence.size();

	for (int i = 0; i < sentence_size; i++)
	{
		for (int j = 0; j < tagger_size; j++)
		{
			double	 pt	  = n_gram_p_transition(dp, bp, taggers, sentence, i, j, type);
			double	 po   = p_observation(taggers[j], sentence[i]);
			dp[j][i] = pt * po;
		}
	}

	result.first = n_gram_p_transition(dp, bp, taggers, sentence, sentence_size, -1, type);

	if (type == "viterbi")
	{	
		StrVec tagged;
		Number backpointer = bp[0][0];
		int bp_h = sentence_size;
		int bp_v;
		while (true)
		{
			int digits = backpointer.get_digits();
			for (int i = digits; i > 0; i--)
				tagged.insert(tagged.begin(), taggers[backpointer[i-1]]);

			bp_h -= (N - 1);
			if (digits < N - 1 || bp_h == 0) break;
			bp_v = backpointer[0];
			backpointer = bp[bp_v][bp_h];
		}

		result.second = tagged;
	}

	return result;
}*/

	/*double POS::n_gram_p_transition(double **dp, Number **bp, const StrVec &taggers,
								const StrVec &sentence, const int h, const int v, const string type)
{
	int tagger_size = taggers.size();
	int sentence_size = sentence.size();
	string tagger = h == sentence_size ? END : taggers[v];
	int cond_size = N - 1;
	int number_of_start_symbols = cond_size > h ? cond_size - h : 0;
	StrVec start(number_of_start_symbols, START);

	if (h == 0)
		return p_transition(start, tagger);

	int number_of_taggers = cond_size - number_of_start_symbols;
	int loop_total = (int)std::pow(tagger_size, number_of_taggers);
	double sum_pt = 0;
	double max_pt = 0;
	Number backpointer(0, tagger_size, number_of_taggers);
	Number index(0, tagger_size, number_of_taggers);
	for (; index.to_decimal() < loop_total; index++)
	{
		double p_dp = h >= cond_size ? dp[index[0]][h-cond_size] : 1;
		if (p_dp == 0) continue;

		StrVec cond = start;
		for (int j = 0; j < number_of_taggers; j++)
			cond.push_back(taggers[index[j]]);

		if (type == "forward")
			sum_pt += p_dp * p_transition(cond, tagger);
		else if (type == "viterbi")
		{
			double pt = p_dp * p_transition(cond, tagger);
			if (pt > max_pt)
			{
				max_pt = pt;
				backpointer = index;
			}
		}
	}
	if (type == "viterbi")
	{
		if (v < sentence_size)
			bp[v][h] = backpointer;
		else
			bp[0][0] = backpointer;
	}

	return sum_pt ? sum_pt : max_pt;
}*/