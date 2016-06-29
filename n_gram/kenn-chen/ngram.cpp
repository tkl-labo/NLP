#include "ngram.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cmath>
//#include <cstdio>
using std::cout;
using std::endl;
using std::string;

using namespace nlp;

const string START = "<s>";
const string END = "</s>";
const string UNK = "<unk>";
const string BREAK = "EOS";
const string DELIMITER = "\t";

Ngram::Ngram(const int n): N(n)
{
	ngram_file = std::to_string(N) + "-gram.store";
}

void Ngram::train(const string train_file)
{
	string line, word;
	TableVectorKey tvkey = {START};
	const int full = N-1;

	std::ifstream fin(train_file);
	if (!fin.is_open()) {
        std::cerr << "ERROR: No such file ("  << train_file << ")" << std::endl;
        std::exit(EXIT_FAILURE);
    }

	while(std::getline(fin, line))
	{	
		std::istringstream iss(line);
		std::getline(iss, word, '\t');
		if (word == BREAK)
		{
			if (tvkey.size() == full)
				insert(tvkey, END);
			tvkey.clear();
			tvkey.push_back(START);
		}
		else 
		{
			if (tvkey.size() < full)
				tvkey.push_back(word);
			else
			{
				insert(tvkey, word);
				tvkey.erase(tvkey.begin());
				tvkey.push_back(word);
			}
		}		
	}
	fin.close();
	store();
}

void Ngram::test(const string test_file)
{
	string line, word;
	TableVectorKey tvkey = {START};
	const int full = N-1;
	long n = 0;
	float total_prob;

	std::ifstream fin(test_file);
	if (!fin.is_open()) {
        std::cerr << "ERROR: No such file ("  << test_file << ")" << std::endl;
        std::exit(EXIT_FAILURE);
    }

	while(std::getline(fin, line))
	{	
		std::istringstream iss(line);
		std::getline(iss, word, '\t');
		if (word == BREAK)
		{
			if (tvkey.size() == full)
			{
				tvkey.push_back(END);
				total_prob += prob_map[hash(tvkey)];
				n++;
			}
			tvkey.clear();
			tvkey.push_back(START);
		}
		else 
		{
			if (tvkey.size() < full)
				tvkey.push_back(word);
			else
			{
				tvkey.push_back(word);
				total_prob += prob_map[hash(tvkey)];
				tvkey.erase(tvkey.begin());
				n++;
			}
		}		
	}
	cout << "perplexity of " << N << "-gram: " << std::pow(10, -total_prob/float(n)) << endl;
	fin.close();	
}

void Ngram::store()
{
	std::ofstream fout(ngram_file);
	for (auto itr1 : table)
	{
		TableKey tkey = itr1.first;
		TableValue entry = itr1.second;
		for (auto itr2 : entry->cells)
		{
			float prob = std::log10((float)itr2.second / (float)entry->count);
			fout << prob << DELIMITER << tkey << DELIMITER << itr2.first << endl;
		}
	}
	fout.close();
}

void Ngram::load()
{
	string line, seq, prob_str;
	float prob;
	std::ifstream fin(ngram_file);
	if (!fin.is_open()) {
        std::cerr << "ERROR: No such file ("  << ngram_file << ")" << std::endl;
        std::exit(EXIT_FAILURE);
    }

	while(std::getline(fin, line))
	{	
		std::istringstream iss(line);
		std::getline(iss, prob_str, '\t');
		prob = std::stof(prob_str);
		std::getline(iss, seq);
		prob_map[seq] = prob;		
	}    

	fin.close();
}

void Ngram::insert(const TableVectorKey& tvkey, const std::string cell)
{
	TableKey tkey = hash(tvkey);
	auto entry = table.find(tkey);
	if (entry == table.end())
		table[tkey] = new Entry(cell);
	else
		entry->second->insert(cell);
}

TableKey Ngram::hash(const TableVectorKey& tvkey)
{
	std::stringstream ss;
	string str_key;
	std::copy(tvkey.begin(), tvkey.end(),std::ostream_iterator<std::string>(ss, "\t"));
	str_key = ss.str();
	str_key.pop_back();
	return str_key;
}

Entry::Entry(const string key): count(1)
{
	cells[key] = 1;
}

void Entry::insert(const string ekey)
{
	auto cell = cells.find(ekey);
	if (cell == cells.end())
		cells[ekey] = 1;
	else
		cell->second++;
	count ++;
}
