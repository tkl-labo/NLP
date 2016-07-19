#include <iostream>
#include <string>
#include <unordered_map>
#include <cstring>

using namespace std;

int main(int argc, char** argv){
	
	if(argc < 1){ cerr << "few args" << endl; exit(1); }
	const int N = stoi(argv[1]); //N-gramのN

	int i,j,k,l;
	unordered_map<string,int> all_n_grams;
	string sentence;
	string n_gram[N];
	int ns = 0;
	while(!cin.eof()){
		getline(cin,sentence);//1文読み込み
		if(sentence == "") continue;
		++ns;
		for(i=0;i<N;++i) n_gram[i] = "<s>";//初期化
		string string_n_gram;
		string::size_type index = 0;
		string::size_type new_index = 0;
		while(1){
			new_index = sentence.find("\t",index);
			if(new_index == string::npos) break;
			for(i=0;i<N-1;++i) n_gram[i] = n_gram[i+1];
			n_gram[N-1] = sentence.substr(index,new_index-index);
			string_n_gram = "";
			for(i=0;i<N;++i) string_n_gram += (n_gram[i]+"\t");
			all_n_grams[string_n_gram] += 1;
			index = new_index+1; 
		}
	}
	unordered_map<string,int>::iterator itr;
	int nwords = 0;
	int V=0;
	for(itr=all_n_grams.begin();itr!=all_n_grams.end();++itr){
		cout << itr->first << itr->second << endl;
		nwords += itr->second;
		++V;
	}
 
	cout << V << endl;
	cout << ns << endl;
	cout << nwords << endl;
	
	return 0;
}