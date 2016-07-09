#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace nlp
{
	class Value;
	using NGMapKey		= std::string;
	using NGMapValue	= Value*;
	using NGMap			= std::unordered_map<NGMapKey, NGMapValue>;
	using TokenMap		= std::unordered_map<std::string, long>;
	using StrVec 		= std::vector<std::string>;

	enum Smoothing {none, laplace};

	class Value
	{
	public:
		long count;
		TokenMap token_map;

		Value(const std::string key);
		Value(const std::string key, const long total_count, const long token_count);
		void insert(const std::string token);
		void insert(const std::string token, const long token_count);
	};

	class Ngram
	{
	private:
		const int N;
		const Smoothing SM;
		long vocab_size;
		NGMap ngram_map;
		std::string ngram_file;
		std::unordered_set<std::string> vocab;

	public:
		Ngram(const int n);
		Ngram(const int n, Smoothing sm);
		~Ngram();
		void train(const std::string train_file);
		void test(const std::string test_file);
	private:
		void insert(const StrVec& tvkey, const std::string token);
		void save();
		void load();
		NGMapKey hash(const StrVec& tvkey);
	};
}