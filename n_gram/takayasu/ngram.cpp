#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>

using namespace std;


unordered_map<string,int> count_n_gram(unordered_map<string,int>& n_gram_map, int N, char* file_name, string mode, int& nwords);
double compute_perplexity(unordered_map<string,int>& n_gram_map, unordered_map<string,int>& n_minus_1_gram_map, int N, char* file_name, int nwords, int V);

double compute_perplexity_1(unordered_map<string,int>& n_gram_map, char * file_name, int nwords,int V);

//laplace_smoothing

int main(int argc, char** argv){
	
	const int N = stoi(argv[1]);
	char* train_file_name = argv[2];
	char* test_file_name = argv[3];
	//ifstream ifs(argv[1]);
	//ifstream ifs("../../../../takayasu/slp/test2.txt");
	//ifstream ifs("../../../../takayasu/slp/20120101.cdr");
	int nwords = 0;
	unordered_map<string,int> n_gram_map;
	unordered_map<string,int> n_minus_1_gram_map;
	unordered_map<string,int> one_gram_map;
	count_n_gram(n_gram_map,N,train_file_name,"train",nwords);
	if(N != 1) count_n_gram(n_minus_1_gram_map,N-1,train_file_name,"train",nwords);
	count_n_gram(one_gram_map,1,train_file_name,"train",nwords);
	
	/*
	int V_train=0;
	for(auto itr = n_gram_map.begin(); itr != n_gram_map.end(); ++itr){
		++V_train;
		//cout << itr->first << "\t" << itr->second << "\n"; //出力
	}
	cout << "V_train = " << V_train << endl;
	*/
	//int nwords = 0
	count_n_gram(n_gram_map,N,test_file_name,"",nwords);
	if(N != 1)count_n_gram(n_minus_1_gram_map,N-1,test_file_name,"",nwords);
	count_n_gram(one_gram_map,1,test_file_name,"count_line", nwords);
	int V = 0;
	for(auto itr = one_gram_map.begin(); itr != one_gram_map.end(); ++itr){
		++V;
		//nwords += itr->second;
		//cout << itr->first << "\t" << itr->second << "\n";
	}
	//nwords -= one_gram_map["<s>,"];
	int nsentences = one_gram_map["</s>,"];
	cout << "V = " << V << endl;
	cout << "nwords = " << nwords << endl;
	cout << "nsentences = " << nsentences << endl;
	//laplace smoothing
	for(auto itr = n_gram_map.begin(); itr != n_gram_map.end(); ++itr){
		itr->second += 1;
	}
	
	//perplexity
	double perplexity;
	if(N != 1){
		perplexity = compute_perplexity(n_gram_map,n_minus_1_gram_map,N, test_file_name,nwords, V);
	}
	else{
		perplexity = compute_perplexity_1(n_gram_map, test_file_name, nwords, V);
	}
	cout << "perplexity = " << perplexity << endl;
	return 0;
}

double compute_perplexity_1(unordered_map<string,int>& n_gram_map, char * file_name, int nwords,int V){
	ifstream ifs(file_name);
	if(!ifs){
		cout << "cannot open" << endl;
		exit(1);
	}
	double perplexity = 1.0;
	int i;
	string buf,word;
	string n_gram_key="";
	while(!ifs.eof()){
		getline(ifs,buf);
		if(buf == "") continue;
		if(buf == "EOS"){
			word = "</s>";
		}
		else{
			word.assign(&buf[0], &buf[buf.find("\t")]);
		}
		n_gram_key = word + ",";
		double x = (nwords+V)/n_gram_map[n_gram_key];
		perplexity *= pow(x,-1.0/nwords);
	}
	return perplexity;
}

double compute_perplexity(unordered_map<string,int>& n_gram_map, unordered_map<string,int>& n_minus_1_gram_map, int N, char* file_name, int nwords, int V){
	double perplexity = 1.0;
	ifstream ifs(file_name);
	if(!ifs){
		cout << "cannot open" << endl;
		exit(1);
	}
	int i,j,k,l;
	
	string n_gram[N];
	for(i=0;i<N;++i){
		n_gram[i] = "<s>";
	}
	string buf;
	string word;
	string n_gram_key;
	string n_minus_1_gram_key;
	while(!ifs.eof()){
		getline(ifs,buf);
		if(buf == "") continue;
		if(word =="</s>"){
			for(i=0;i<N;++i){
				n_gram[i] = "<s>";
			}
		}
		if(buf == "EOS"){
			word = "</s>";
		}
		else{
			word.assign(&buf[0], &buf[buf.find("\t")]); //切り出し
		}
		for(i=0;i<N-1;++i){
			n_gram[i] = n_gram[i+1];
		}
		n_gram[N-1] = word;
		n_gram_key = "";
		for(i=0;i<N;++i){
			n_gram_key += (n_gram[i]+ ",") ;
		}
		n_minus_1_gram_key="";
		for(i=0;i<N-1;++i){
			n_minus_1_gram_key += (n_gram[i]+ ",") ;
		}
		double x = (n_minus_1_gram_map[n_minus_1_gram_key]+V)/n_gram_map[n_gram_key];
		perplexity *= pow(x,-1.0/nwords);
	}
	return perplexity;
}

unordered_map<string,int> count_n_gram(unordered_map<string,int>& n_gram_map, int N, char* file_name, string mode, int& nwords){
		ifstream ifs(file_name);
	//ofstream ofs(argv[3]);
	if(!ifs){
		cout << "cannot open" << endl;
		exit(1);
	}
	
	
	int i,j,k,l;
	
	string n_gram[N];
	for(i=0;i<N;++i){
		n_gram[i] = "<s>";
	}
	string buf;
	string word;
	string n_gram_key;
	while(!ifs.eof()){
		getline(ifs,buf);
		if(buf == "") continue;
		if(word =="</s>"){
			for(i=0;i<N;++i){
				n_gram[i] = "<s>";
			}
		}
		if(buf == "EOS"){
			word = "</s>";
		}
		else{
			word.assign(&buf[0], &buf[buf.find("\t")]); //切り出し
		}
		for(i=0;i<N-1;++i){
			n_gram[i] = n_gram[i+1];
		}
		n_gram[N-1] = word;
		n_gram_key = "";
		for(i=0;i<N;++i){
			n_gram_key += (n_gram[i]+ ",") ;
		}
		if(mode == "train"){
			n_gram_map[n_gram_key]+=1;
		}
		else{
			if(mode == "count_line"){
				nwords += 1;
			}
			n_gram_map[n_gram_key]+=0; //キーと値のペアがなければ0で作成、あればそのまま
		}
		//cout << n_gram_key << n_gram_map[n_gram_key] << endl;
	}
	return n_gram_map;
}
