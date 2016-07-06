#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <cmath>
using namespace std;

/*
学習データを読み込む(n_gram,n-1_gram,1-gram)
テストデータ(文章)を読み込み、1-gramを使ってVを決める
テストデータを読み込み、pとperplexityを計算

*/


int main(int argc, char** argv){
	
	if(argc < 1){ cerr << "few args" << endl; exit(1); }
	const int N = stoi(argv[1]);
	
	ifstream ifs_train_1("train_1_grams");
	ifstream ifs_train("train_"+to_string(N)+"_grams");
	ifstream ifs_train_n_1("train_"+to_string(N-1)+"_grams");
	ifstream ifs_test("test_sentences");
	if(!ifs_train || !ifs_test){cerr<< "cannot open" << endl; exit(1);}
	int i,j,k,l;
	unordered_map<string,int> train_1_grams;
	unordered_map<string,int> train_n_grams;
	unordered_map<string,int> train_n_1_grams;
	//unordered_map<string,int> all_n-1_grams;
	//unordered_map<string,int> vocabulary_count;
	
	//学習データの読み込み(ファイルから)
	string sbuf;
	string n_gram;
	int counted_num;
	string::size_type index;
	while(!ifs_train.eof()){
		getline(ifs_train,sbuf);
		if(sbuf=="") continue;
		index = sbuf.rfind("\t");
		n_gram = sbuf.substr(index);
		counted_num = stoi(sbuf.substr(index+1,10));
		train_n_grams[n_gram] = counted_num;
	}
	while(!ifs_train_n_1.eof()){
		getline(ifs_train_n_1,sbuf);
		if(sbuf=="") continue;
		index = sbuf.rfind("\t");
		n_gram = sbuf.substr(index);
		counted_num = stoi(sbuf.substr(index+1,10));
		train_n_1_grams[n_gram] = counted_num;
	}
	while(!ifs_train_1.eof()){
		getline(ifs_train_1,sbuf);
		if(sbuf=="") continue;
		index = sbuf.rfind("\t");
		n_gram = sbuf.substr(index);
		counted_num = stoi(sbuf.substr(index+1,10));
		train_1_grams[n_gram] = counted_num;
	}
	//テストデータの語彙をチェック
	string sentence;
	string word;
	int test_N;
	while(!ifs_test.eof()){
		getline(ifs_test,sentence);
		if(sentence == "") continue;
		string::size_type index = 0;
		string::size_type new_index = 0;
		while(1){
			new_index = sentence.find("\t",index);
			if(new_index == string::npos) break;
			word = sentence.substr(index,new_index-index);
			train_1_grams[word] += 0;
			test_N += 1;
		}
	}
	cout << "test_N" << test_N << endl;
	//語彙数のカウント
	int V;
	unordered_map<string,int>::iterator itr;
	for(itr=train_1_grams.begin();itr!=train_1_grams.end();++itr){
		++V;
	}
	cout << "voc = " << V << endl;
	train_1_grams.clear();
	
	//テストデータの読み込み
	string test_n_gram[N];
	int C,C1;
	double P;
	double sum_logP;
	while(!ifs_test.eof()){
		getline(ifs_test,sentence);
		if(sentence == "") continue;
		for(i=0;i<N;++i) test_n_gram[i] = "<s>";
		string string_n_gram, string_n_1_gram;
		string::size_type index = 0;
		string::size_type new_index = 0;
		while(1){
			new_index = sentence.find("\t",index);
			if(new_index == string::npos) break;
			for(i=0;i<N-1;++i) test_n_gram[i] = test_n_gram[i+1];
			test_n_gram[N-1] = sentence.substr(index,new_index-index);
			string_n_gram = "";
			for(i=0;i<N;++i) string_n_gram += (test_n_gram[i]+"\t");
			for(i=0;i<N-1;++i) string_n_1_gram += (test_n_gram[i]+"\t");
			C = train_n_grams[string_n_gram]+1;
			C1 = train_n_1_grams[string_n_1_gram]+V;
			P = (double)C / C1;
			sum_logP += log(P);
			index = new_index+1; 
		}
	}
	double pp = exp(-sum_logP/test_N);//pp= ([pi] p)^(-1/N) log(pp) = (-1/N) sigma log(p)
	cout << "perplexity = " << pp << endl;
	
	
	return 0;
}