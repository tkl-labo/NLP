#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<unordered_map>
using namespace std;

int main(){
	ifstream ifst2("tag_2gram.txt");
	ifstream ifstr("train2.txt");
	ifstream ifste("test2.txt");
	ofstream ofsde("debug_1gram.txt");
	
	unordered_map<string,int>count_1gram;
	unordered_map<string,int>count_2gram;
	unordered_map<string,int>word_tag;
	vector<string>tags;
	vector<int>counts;
	
	int i,j,k,l;
	
	string buf;
	string word,tag;
	while(1){
		getline(ifst2,buf);
		if(ifst2.eof()) break;
		count_2gram[buf]+=1;
		count_1gram[buf.substr(0,buf.find(" "))]+=1;
	}
	ifst2.close();
	count_1gram["</s>"]=0;
	//int sum=0;
	int n_tags=0;
	unordered_map<string,int>::const_iterator itr;
	for(itr=count_1gram.begin(); itr!=count_2gram.end();++itr){
		//ofsde << itr->first << " " << itr->second << endl;
		n_tags += 1;
		tags.push_back(itr->first);
		counts.push_back(itr->second);
	}
	cout << n_tags << endl;
	
	
	while(1){
		getline(ifstr,buf);
		if(ifstr.eof()) break;
		if(buf=="") continue;
		word_tag[buf]+=1;
	}
	ifstr.close();
	
	
	
	
	//string prev_tag = "<s>";
	/*
	vector<vector<double>> forward;
	vector<double> v;
	v.reserve(n_tags);
	itr = count_1gram.begin();
	for(j=0;j<n_tags;++j){
		v.push_back( ((double)count_2gram["<s>"+" "+(itr->first)+" "]) / count_1gram["<s>"] * (word_tag[word+" "+(itr->first)]+1) / count_1gram[(itr->first)]);
		++itr;
	}
	v[n_tags] = 0;
	forward.push_back(v);
	*/
	cout << "hina" << endl;
	while(1){
		getline(ifste,buf);
		if(ifste.eof()) break;
		if(buf=="") continue;
		vector<vector<double>> forward;
		vector<double> v;
		v.reserve(n_tags);
		for(j=0;j<n_tags;++j){
			v[j] = ( ((double)count_2gram["<s> "+tags[j]+" "]) / count_1gram["<s>"] * (word_tag[word+" "+tags[j]]) / count_1gram[tags[j]] );
		}
		//v[n_tags] = 0;
		forward.push_back(v);
		i = 1;
		for(;;++i){
			getline(ifste,buf);
			if(buf=="") break;
			word = buf.substr(0,buf.find(" "));
			for(j=0;j<n_tags;++j){
				double x = 0;
				for(k=0;k<n_tags;++k){
					x += forward[i-1][k] * ( ((double)count_2gram[tags[k]+" "+tags[j]+" "]) / counts[k] * (word_tag[word+" "+tags[j]]) / counts[j] );
				}
				v[j] = x;
			}
			forward.push_back(v);
		}
		for(l=0;l<i;+i){
			for(k=0;k<n_tags;++k){
				cout << forward[i][k] << " ";
			}
			cout << endl;
		}
		break;
	}

}