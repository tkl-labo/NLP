#pragma once
#include "util.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace nlp
{
	class Counter;
	using TableKey	 = std::string;
	using TableValue = Counter*;
	using TableMap	 = std::unordered_map<TableKey, TableValue>;
	using CounterMap = std::unordered_map<std::string, long>;
	using StrVec	 = std::vector<std::string>;
	using pos_pair	 = std::pair<double, StrVec>;

	class Counter
	{
	public:
		long		count;
		CounterMap	counter;

				Counter(const std::string key);
				Counter(const std::string key, const long total_count, const long target_count);
		void	insert(const std::string target);
		void	insert(const std::string target, const long target_count);
	};

	class Table
	{
	public:
		TableMap table;

				Table();
				Table(std::string serialized_file);
				~Table();
		void	insert(const TableKey key, const std::string target);
		void	insert(const TableKey key, const std::string target, const long total_count, const long count);
		void	serialize(std::string file_name);
		void	deserialize(std::string file_name);
		StrVec	keys();
	};

	class POS
	{
	private:
		const int						N;
		long							vocab_size;
		std::unordered_set<std::string>	vocab;
		std::string						transition_table_file;
		std::string						observation_table_file;
		std::string						param_save_file;
		std::vector<StrVec>				test_token_list;
		std::vector<StrVec>				test_tagger_list;
		Table							transition_table;
		Table							observation_table;

	public:
					POS(const int n);
		void		train(const std::string train_file);
		void		test(const std::string test_file);
	private:
		void		read_test_data(std::string test_file);
		double		accuracy(StrVec sv1, StrVec sv2);
		TableKey	hash(const StrVec& v);
		void		forward(StrVec sentence);
		StrVec		viterbi(StrVec sentence);
		pos_pair	dp_algo(StrVec sentence, const std::string type);
		double		p_transition(StrVec cond, std::string tagger);
		double		n_gram_p_transition(double **dp, Number **bp, const StrVec &taggers, const StrVec &sentence, const int h, const int v, const std::string type);
		double		p_observation(std::string tagger, std::string token);
		void		save_param(std::string file_name);
		void		load_param(std::string file_name);
	};
}