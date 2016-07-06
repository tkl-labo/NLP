#pragma once
#include <vector>
#include <string>
#include <unordered_map>

namespace nlp
{
	class Entry;
	using TableKey		 = std::string;
	using TableValue	 = Entry*;
	using TableVectorKey = std::vector<std::string>;
	using TableType		 = std::unordered_map<TableKey, TableValue>;

	enum Smoothing {none, laplace};

	class Entry
	{
	public:
		int count;
		const Smoothing SM;
		std::unordered_map<std::string, int> cells;

		Entry(const std::string key, const Smoothing sm);
		void insert(std::string key);
	};

	class Ngram
	{
	private:
		const int N;
		const Smoothing SM;
		TableType table;
		std::string ngram_file;
		std::unordered_map<std::string, double> prob_map;

	public:
		Ngram(const int n);
		Ngram(const int n, Smoothing sm);
		~Ngram();
		void train(const std::string train_file);
		void test(const std::string test_file);
	private:
		void insert(const TableVectorKey& tvkey, const std::string cell);
		void save();
		void load();
		TableKey hash(const TableVectorKey& tvkey);
	};
}