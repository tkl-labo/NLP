#pragma once
#include <vector>
#include <unordered_map>

namespace nlp
{
class Entry;
typedef std::vector<std::string> TableVectorKey;
typedef std::string TableKey;
typedef Entry* TableValue;
typedef std::unordered_map<TableKey, TableValue> TableType;

class Entry
{
public:
	int count;
	std::unordered_map<std::string, int> cells;

	Entry(const std::string key);
	void insert(std::string key);
};

class Ngram
{
private:
	const int N;
	std::string ngram_file;
	TableType table;
	std::unordered_map<std::string, float> prob_map;

public:
	Ngram(const int n);
	void train(const std::string train_file);
	void test(const std::string test_file);
private:
	void insert(const TableVectorKey& tvkey, const std::string cell);
	void store();
	void load();
	TableKey hash(const TableVectorKey& tvkey);

};
}