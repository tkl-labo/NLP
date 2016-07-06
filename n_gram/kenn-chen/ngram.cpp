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
const string DELIMITER	= "\t";

Ngram::Ngram(const int n) : N(n), SM(none)
{
	ngram_file = std::to_string(N) + "-gram.save";
}

Ngram::Ngram(const int n, Smoothing sm) : N(n), SM(sm)
{
	if (sm == laplace)
		ngram_file = std::to_string(N) + "-gram-laplace.save";
	else
		ngram_file = std::to_string(N) + "-gram.save";
}

Ngram::~Ngram()
{
	for (auto entry = table.begin(); entry != table.end(); entry++)
		delete entry->second;
}

void Ngram::train(const string train_file)
{
	const int KEY_SIZE = N - 1;
	TableVectorKey vk_start(KEY_SIZE, START);
	TableVectorKey vk = vk_start;
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
		string word = "";
		std::getline(iss, word, '\t');
		if (word == BREAK)
		{
			insert(vk, END);
			if (KEY_SIZE > 0)
				vk = vk_start;
		}
		else
		{
			insert(vk, word);
			if (KEY_SIZE > 0)
			{
				vk.erase(vk.begin());
				vk.push_back(word);
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
	TableVectorKey vk_start(KEY_SIZE, START);
	TableVectorKey vk = vk_start;

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
	long n = 0;
	double total_prob = 0;
	while (std::getline(*fin, line))
	{
		n++;
		std::istringstream iss(line);
		string word = "";
		std::getline(iss, word, '\t');
		if (word == BREAK)
		{
			vk.push_back(END);
			if (prob_map.find(hash(vk)) != prob_map.end())
				total_prob += prob_map[hash(vk)];
			else
				total_prob += std::log10(1 / n);
			vk = vk_start;
		}
		else
		{
			vk.push_back(word);
			total_prob += prob_map[hash(vk)];
			vk.erase(vk.begin());
		}
	}
	if (test_file != "")
	{
		((std::ifstream*)fin)->close();
		delete fin;
	}
	double perplexity = std::pow(10, -total_prob / double(n));
	if (SM == laplace)
		cout << "perplexity of " << N << "-gram-laplace: " << perplexity << endl;
	else
		cout << "perplexity of " << N << "-gram: " << perplexity << endl;
		
}

void Ngram::save()
{
	std::ofstream fout(ngram_file);
	for (auto key_entry : table)
	{
		TableKey   tkey   = key_entry.first;
		TableValue entry  = key_entry.second;
		double total_count = (double)entry->count;
		for (auto cell : entry->cells)
		{
			string word = cell.first;
			double count	= (double)cell.second;
			double prob = std::log10(count / total_count);
			if (tkey.size() > 0)
				fout << prob << DELIMITER << tkey << DELIMITER << word << endl;
			else
				fout << prob << DELIMITER << word << endl;
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
	while (std::getline(fin, line))
	{
		std::istringstream iss(line);
		string prob_str = "";
		std::getline(iss, prob_str, '\t');
		double prob = std::stof(prob_str);
		string seq = "";
		std::getline(iss, seq);
		prob_map[seq] = prob;
	}
	fin.close();
}

void Ngram::insert(const TableVectorKey& vk, const std::string cell)
{
	TableKey tkey = hash(vk);
	auto entry = table.find(tkey);
	if (entry == table.end())
		table[tkey] = new Entry(cell, SM);
	else
		entry->second->insert(cell);
}

TableKey Ngram::hash(const TableVectorKey& vk)
{
	std::stringstream ss;
	string str_key;
	std::copy(vk.begin(), vk.end(), std::ostream_iterator<std::string>(ss, "\t"));
	str_key = ss.str();
	//str_key equals 0 in unigram.
	if (str_key.size() > 0)
		str_key.pop_back();
	return str_key;
}

Entry::Entry(const string key, const Smoothing sm) : SM(sm)
{
	switch (sm)
	{
	case laplace:
		cells[key] = 2;
		count = 2;
		break;
	default:
		cells[key] = 1;
		count = 1;
		break;
	}
}

void Entry::insert(const string entry_key)
{
	auto cell = cells.find(entry_key);
	if (cell == cells.end())
	{
		switch (SM)
		{
		case laplace:
			cells[entry_key] = 2;
			count += 2;
		default:
			cells[entry_key] = 1;
			count++;
			break;
		}
	}
	else
	{
		cell->second++;
		count++;
	}
}
