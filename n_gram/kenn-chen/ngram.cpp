#include "ngram.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cmath>

using std::cout;
using std::endl;
using std::string;

using namespace nlp;

const string START		= "<s>";
const string END		= "</s>";
const string UNK		= "<unk>";
const string BREAK		= "EOS";
const string EMPTYKEY	= "";
const string DELIMITER	= "\t";


Ngram::Ngram(const int n) : Ngram(n, none) {}

Ngram::Ngram(const int n, Smoothing sm) : N(n), SM(sm)
{
	ngram_file = std::to_string(N) + "-gram.save";
	vocab = {END, UNK};
}

Ngram::~Ngram()
{
	for (auto value = ngram_map.begin(); value != ngram_map.end(); value++)
		delete value->second;
	ngram_map.clear();
}

void Ngram::train(const string train_file)
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
		std::istringstream iss(line);
		string token = "";
		std::getline(iss, token, '\t');

		if (token == BREAK)
		{
			insert(vkey, END);
			if (KEY_SIZE > 0)
				vkey = vkey_start;
		}
		else
		{
			vocab.insert(token);
			insert(vkey, token);
			if (KEY_SIZE > 0)
			{
				vkey.erase(vkey.begin());
				vkey.push_back(token);
			}
		}
	}
	if (train_file != "")
	{
		((std::ifstream*)fin)->close();
		delete fin;
	}
	else
		std::cin.clear();
	save();
}

void Ngram::test(const string test_file)
{
	load();
	
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

	string line = "";
	double pp = 0;
	long 	n = 0;
	while (std::getline(*fin, line))
	{	
		n++;
		std::istringstream iss(line);
		string token;
		std::getline(iss, token, '\t');
		if (token == BREAK) token = END;

		auto ngram_pair = ngram_map.find(hash(vkey));
		if (ngram_pair == ngram_map.end())
			pp += std::log10(1/(double)vocab_size);
		else
		{
			NGMapValue value = ngram_pair->second;
			long total_count = value->count;
			auto token_pair  = value->token_map.find(token);
			if (token_pair == value->token_map.end())
				pp += std::log10(1 /(double)(total_count+vocab_size));
			else
			{
				long count = token_pair->second;
				pp += std::log10((double)(count+1) / (double)(total_count+vocab_size));
			}
		}

		if (token == END)
			vkey = vkey_start;
		else
		{
			vkey.push_back(token);
			vkey.erase(vkey.begin());
		}
	}

	if (test_file != "")
	{
		((std::ifstream*)fin)->close();
		delete fin;
	}

	pp = std::pow(10, -pp/double(n));
	if (SM == laplace)
		cout << "perplexity of " << N << "-gram-laplace: " << std::fixed << pp << endl;
	else
		cout << "perplexity of " << N << "-gram: " << std::fixed << pp << endl;
}

void Ngram::save()
{
	std::ofstream fout(ngram_file);
	//save the total_count in the first line in the ngram file
	fout << vocab.size() << endl;
	for (auto ngram_pair : ngram_map)
	{
		NGMapKey   key   = ngram_pair.first;
		NGMapValue value = ngram_pair.second;
		long total_count = value->count;
		for (auto token_pair : value->token_map)
		{
			string token = token_pair.first;
			long   count = token_pair.second;
			fout << count << DELIMITER << total_count << DELIMITER <<   token   << DELIMITER << key << endl;
		}
	}
	fout.close();
}

void Ngram::load()
{
	std::ifstream fin(ngram_file);
	if (!fin.is_open()) 
	{
		std::cerr << "ERROR:" << N <<"-gram file not found." << std::endl;
		std::exit(EXIT_FAILURE);
	}

	string line = "";
	std::getline(fin, line);
	vocab_size = std::stol(line);

	while (std::getline(fin, line))
	{
		std::istringstream iss(line);
		string s = "";
		
		std::getline(iss, s, '\t');
		long count = std::stol(s);
		std::getline(iss, s, '\t');
		long total_count = std::stol(s);

		string key, token;
		std::getline(iss, token, '\t');
		std::getline(iss, key);

		auto ngram_pair = ngram_map.find(key);
		if (ngram_pair == ngram_map.end())
			ngram_map[key] = new Value(token, total_count, count);
		else
			ngram_pair->second->insert(token, count);
	}
	fin.close();
}

void Ngram::insert(const StrVec& vkey, const std::string token)
{
	NGMapKey key = hash(vkey);
	auto ngram_pair = ngram_map.find(key);
	if (ngram_pair == ngram_map.end())
		ngram_map[key] = new Value(token);
	else
		ngram_pair->second->insert(token);
}

NGMapKey Ngram::hash(const StrVec& vkey)
{
	std::stringstream ss;
	string str_key;
	std::copy(vkey.begin(), vkey.end(), std::ostream_iterator<std::string>(ss, "\t"));
	str_key = ss.str();

	//str_key equals 0 in unigram.
	if (str_key.size() > 0)
		str_key.pop_back();
	return str_key;
}

Value::Value(const string token)
{
	token_map[token] = 1;
	count = 1;
}

Value::Value(const string token, const long total_count, const long token_count)
{
	token_map[token] = token_count;
	count = total_count;
}

void Value::insert(const string token)
{
	auto token_pair = token_map.find(token);
	if (token_pair == token_map.end())
	{
		token_map[token] = 1;
		count++;
	}
	else
	{
		token_pair->second++;
		count++;
	}
}

void Value::insert(const string token, const long token_count)
{
	token_map[token] = token_count;
}