#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <cmath>
#include <sstream>

using namespace std;

int main(int argc, char** argv){
	
	if(argc < 1){ cerr << "few args" << endl; exit(1); }
	const int N = stoi(argv[1]);
	
	stringstream ss,ss1;
	ss << N-1;
	ss1 << N;
	ifstream ifs_train_1("train_1_grams.csv");
	ifstream ifs_train("train_"+ss1.str()+"_grams.csv");
	ifstream ifs_train_n_1("train_"+ss.str()+"_grams");
	//ifstream ifs_test("test_sentences");
	ifstream ifs_test("20130101.csv");
	int i,j,k,l;
	unordered_map<string,int> train_n_grams;
	unordered_map<string,int> train_n_1_grams;
	unordered_map<string,int> train_1_grams;

	//学習データの読み込み(ファイルから)
	string sbuf;
	string n_gram;
	int counted_num;
	string::size_type index;
	
	//n_gram
	while(!ifs_train.eof()){
		getline(ifs_train,sbuf);
		if(sbuf=="") continue;
		index = sbuf.rfind("\t");
		n_gram = sbuf.substr(0,index+1);
		counted_num = stoi(sbuf.substr(index+1,10));
		train_n_grams[n_gram] = counted_num;
	}
	//n-1_gram if N!=1
	if(N != 1){
		while(!ifs_train_n_1.eof()){
			getline(ifs_train_n_1,sbuf);
			if(sbuf=="") continue;
			index = sbuf.rfind("\t");
			n_gram = sbuf.substr(0,index+1);
			counted_num = stoi(sbuf.substr(index+1,10));
			train_n_1_grams[n_gram] = counted_num;
		}
	}
	//1_gram
	while(!ifs_train_1.eof()){
		getline(ifs_train_1,sbuf);
		if(sbuf=="") continue;
		index = sbuf.rfind("\t");
		n_gram = sbuf.substr(0,index+1);
		counted_num = stoi(sbuf.substr(index+1,10));
		train_1_grams[n_gram] = counted_num;
	}
	
	ifs_train.close();
	ifs_train_1.close();
	ifs_train_n_1.close();
	
	//テストデータの語彙をチェック

	
	
	//語彙数のカウント
	int V=0;//種類数
	int nwords = 0;//全単語数
	unordered_map<string,int>::iterator itr;
	for(itr=train_1_grams.begin();itr!=train_1_grams.end();++itr){
		++V;
		nwords += itr->second;
	}
	cout << "voc(includes </s>, but not UNK) = " << V << endl;
	cout << "nwords = " << nwords << endl;
	train_1_grams.clear();
	
	if(N == 2) train_n_1_grams["<s>\t"] = train_n_1_grams["</s>\t"];
	//テストデータの読み込み
	string test_n_gram[N];
	string sentence;
	string word;
	int C,C1;
	int test_N=0;
	double P;
	double sum_logP=0;
	double pp2 = 1;
	ifs_test.clear();
	ifs_test.seekg(0);
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
			string_n_1_gram = "";
			for(i=0;i<N;++i) string_n_gram += (test_n_gram[i]+"\t");
			for(i=0;i<N-1;++i) string_n_1_gram += (test_n_gram[i]+"\t");
			//cout << string_n_gram << endl;
			C = train_n_grams[string_n_gram]+1;
			if(N!=1) C1 = train_n_1_grams[string_n_1_gram]+ V + 1;
			else C1 = nwords + V + 1;
			P = (double)C / C1;
			cout << 1.0/P << endl;
			sum_logP += log(P);
			index = new_index+1;
			++test_N;
		}
	}
	cout << "test_N = " << test_N << endl;
	double pp = exp(-sum_logP/test_N);//pp= ([pi] p)^(-1/N) log(pp) = (-1/N) sigma log(p)
	cout << "perplexity = " << pp << endl;
	
	return 0;
}