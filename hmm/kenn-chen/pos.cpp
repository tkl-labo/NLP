#include "pos.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cmath>
#include <cstdio>

using std::cout;
using std::endl;
using std::string;

using namespace nlp;

const string START		= "<s>";
const string END		= "</s>";
const string UNK		= "<unk>";
const string BREAK		= "";
const string DELIMITER	= " ";
const char	DELIMITER_C = ' ';

POS::POS(const int n) : N(n)
{
	vocab_size				= 2;
	vocab					= {END, UNK};
	transition_table_file	= "transition_table-" + std::to_string(N) + "-gram.save";
	observation_table_file	= "observation_table.save";
	param_save_file			= "param.save";
}

void POS::train(const string train_file)
{
	const int KEY_SIZE = N - 1;
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
			transition_table.insert(hash(vkey), END);
			if (KEY_SIZE > 0)
				vkey = vkey_start;
		}
		else
		{
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
	
	vocab_size = vocab.size();

	save_param(param_save_file);
	observation_table.serialize(observation_table_file);
	transition_table.serialize(transition_table_file);
}

void POS::test(const string test_file)
{
	load_param(param_save_file);
	observation_table.deserialize(observation_table_file);
	transition_table.deserialize(transition_table_file);

	read_test_data(test_file);

	double acc = 0;
	size_t size = test_tagger_list.size();
	for (int i = 0; i < size; i++)
	{
		StrVec predict_taggers = viterbi(test_token_list[i]);
		double a = accuracy(test_tagger_list[i], predict_taggers);
		cout << "acc: " << a << endl;
		acc += accuracy(test_tagger_list[i], predict_taggers);
	}
	acc /= size;

	cout << "accuracy: " << acc << endl;

	//StrVec sv {"Rockwell", "International", "Corp", "'s", "Tulsa", "unit", "said","it","signed","a","tentative","agreement","extending",
	//		  "its", "contract", "with", "Boeing", "Co.NNP", "to", "provide", "structural", "parts", "for", "Boeing", "'s", "747", "jetliners", "."};
	//StrVec sv{ "He", "reckons", "the", "current", "account", "deficit", "will", "narrow",
	//	"to", "only", "# #", "1.8", "billion", "in", "September", "." };
	
	//viterbi(sv);

	//StrVec sv1 {"nice", "I", "are", "jump"};
	//viterbi(sv1);

	//StrVec sv2{ "I", "am", "good", "people" };
	//viterbi(sv2);
}

double POS::accuracy(StrVec sv1, StrVec sv2)
{
	size_t size = sv1.size();
	unsigned count = 0;
	for (int i = 0; i < size; i++)
		if (sv1[i] == sv2[i])
			count++;
	return double(count) / double(size);
}

void POS::read_test_data(string test_file)
{
	const int KEY_SIZE = N - 1;
	StrVec	vkey_start(KEY_SIZE, START);
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
	StrVec tokens, taggers;
	while (std::getline(*fin, line))
	{
		if (line == BREAK)
		{
			test_token_list.push_back(tokens);
			test_tagger_list.push_back(taggers);
			tokens.clear();
			taggers.clear();
		}
		else
		{
			std::istringstream iss(line);
			string str;
			std::getline(iss, str, DELIMITER_C);
			tokens.push_back(str);
			std::getline(iss, str, DELIMITER_C);
			taggers.push_back(str);
		}
	}

	if (test_file != "")
	{
		((std::ifstream*)fin)->close();
		delete fin;
	}
}

void POS::forward(StrVec sentence)
{
	double prob = dp_algo(sentence, "forward").first;
	cout << "likelihood: " << prob << endl;
}

StrVec POS::viterbi(StrVec sentence)
{
	pos_pair result = dp_algo(sentence, "viterbi");
	cout << "taggers: " << hash(result.second) << endl
		 << "probability: " << result.first << endl;
	return result.second;
}

pos_pair POS::dp_algo(StrVec sentence, const string type)
{
	pos_pair  result;
	StrVec	  taggers		= observation_table.keys();
	const int tagger_size	= taggers.size();
	const int sentence_size = sentence.size();

	double **dp = new double *[tagger_size];
	for (int i = 0; i < tagger_size; i++)
		dp[i] = new double[sentence_size];

	Number **bp = nullptr;
	if (type == "viterbi")
	{
		bp = new Number *[tagger_size];
		for (int i = 0; i < tagger_size; i++)
			bp[i] = new Number[sentence_size];
	}
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

	for (int i = 0; i < tagger_size; i++)
		delete[] dp[i];
	delete[] dp;

	if (type == "viterbi")
	{	
		StrVec	tagged;
		Number	backpointer = bp[0][0];
		int		bp_h = sentence_size;
		int		bp_v;
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

		for (int i = 0; i < tagger_size; i++)
			delete[] bp[i];
		delete[] bp;
	}

	return result;
}

double POS::n_gram_p_transition(double **dp, Number **bp, const StrVec &taggers,
								const StrVec &sentence, const int h, const int v, const string type)
{
	string	 tagger = v == -1 ? END : taggers[v];
	int		 tagger_size = taggers.size();
	int		 cond_size = N - 1;
	int		 number_of_start_symbols = cond_size > h ? cond_size - h : 0;
	StrVec	 start(number_of_start_symbols, START);

	if (h == 0)
		return p_transition(start, tagger);

	int		number_of_taggers = cond_size - number_of_start_symbols;
	int		loop_total = (int)std::pow(tagger_size, number_of_taggers);
	double	sum_pt = 0;
	double	max_pt = 0;
	Number	backpointer(0, tagger_size, number_of_taggers);
	Number	index(0, tagger_size, number_of_taggers);
	for (; index.to_decimal() < loop_total; index++)
	{
		double p_0 = h >= cond_size ? dp[index[0]][h-cond_size] : 1;
		if (p_0 == 0) continue;

		StrVec cond = start;
		for (int j = 0; j < number_of_taggers; j++)
			cond.push_back(taggers[index[j]]);

		if (type == "forward")
			sum_pt += p_0 * p_transition(cond, tagger);
		else if (type == "viterbi")
		{
			double pt = p_0 * p_transition(cond, tagger);
			if (pt > max_pt)
			{
				max_pt = pt;
				backpointer = index;
			}
		}
	}
	if (type == "viterbi")
	{
		if (v != -1)
			bp[v][h] = backpointer;
		else
			bp[0][0] = backpointer;
	}

	return sum_pt ? sum_pt : max_pt ? max_pt : 0;
}

double POS::p_transition(StrVec cond, string tagger)
{
	TableMap &table = transition_table.table;
	auto key_value  = table.find(hash(cond));
	if (key_value == table.end())
		return 0;
	
	CounterMap &counter = key_value->second->counter;
	long total_count	= key_value->second->count;
	auto counter_pair	= counter.find(tagger);
	if (counter_pair == counter.end())
		return 0;

	long count = counter_pair->second;
	return (double)count / (double)total_count;
}

double POS::p_observation(string tagger, string token)
{
	TableMap &table = observation_table.table;
	auto key_value = table.find(tagger);
	if (key_value == table.end())
		return 1 / (double)vocab_size;

	CounterMap &counter = key_value->second->counter;
	long total_count = key_value->second->count;
	auto counter_pair = counter.find(token);
	if (counter_pair == counter.end())
		return 1 / ((double)(total_count + vocab_size));

	long count = counter_pair->second;
	return (double)(count + 1) / ((double)(total_count + vocab_size));
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

void POS::save_param(string file_name)
{
	std::ofstream fout(file_name);
	fout << "vocab_size" << DELIMITER << vocab_size << endl;
	fout.close();
}

void POS::load_param(string file_name)
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
		std::istringstream iss(line);
		string str;
		std::getline(iss, str, DELIMITER_C);
		std::getline(iss, str);
		vocab_size = std::stol(str);
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