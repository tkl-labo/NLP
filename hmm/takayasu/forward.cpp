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
	//ofstream ofsde("debug_1gram.txt");
	ofstream ofst("output_tag.txt");
	
	unordered_map<string,int>count_1gram;
	unordered_map<string,int>count_2gram;
	unordered_map<string,int>word_tag;
	unordered_map<string,int>count_words;
	vector<string>tags;
	vector<int>counts;
	int count_start=0;
	int n_correct=0;
	int n_wrong=0;
	int i,j,k,l;
	
	string buf;
	string word,tag;
	
	//タグの2gramのファイルの読み込み→出現回数(1gram)も確認
	while(1){
		getline(ifst2,buf);
		if(ifst2.eof()) break;
		count_2gram[buf]+=1;
		word = buf.substr(0,buf.find(" "));
		if(word!="<s>") count_1gram[word]+=1;
		else count_start+=1;
	}
	ifst2.close();
	//count_1gram["</s>"]=0;
	//int sum=0;
	int n_tags=0;//n_tagsには<s>と</s>を含まない
	unordered_map<string,int>::const_iterator itr;
	for(itr=count_1gram.begin(); itr!=count_1gram.end();++itr){
		//ofsde << itr->first << " " << itr->second << endl;
		n_tags += 1;
		tags.push_back(itr->first);
		counts.push_back(itr->second);
	}
	cout << "n_tags = " << n_tags << endl;
	
	//単語とタグのセットを読み込み
	while(1){
		getline(ifstr,buf);
		if(ifstr.eof()) break;
		if(buf=="") continue;
		word_tag[buf]+=1;
		word = buf.substr(0,buf.find(" "));
		count_words[word]+=1;
	}
	ifstr.close();
	//語彙のカウント
	int V = 0; //Vに<UNK>は含まない
	for(itr=count_words.begin();itr!=count_words.end();++itr) ++V;
	cout << "V = " << V << endl;
	
	
	int loop = 0;
	while(1){//文のループ
		//if(loop>0) break;
		++loop;
		getline(ifste,buf);
		if(ifste.eof()) break;
		if(buf=="") continue;
		//初期化
		vector<vector<double>> forward;
		vector<vector<double>> viterbi;
		vector<vector<int>> backpointer;
		vector<double>vf(n_tags);
		vector<double>vv(n_tags);
		vector<int>vb(n_tags);
		vector<string>output_tags;
		for(j=0;j<n_tags;++j){
			double a,b;
			a = (double)(count_2gram["<s> "+tags[j]+" "]) / count_start;
			b = (double)(word_tag[word+" "+tags[j]]+1) / (counts[j]+V+1) ;
			vf[j] = a * b;
			vv[j] = vf[j];
			vb[j] = 0;
		}
		forward.push_back(vf);
		viterbi.push_back(vv);
		backpointer.push_back(vb);
		
		//for(j=0;j<n_tags;++j) cout << forward[0][j] << "+" ;
		//cout << endl;
		int i2 = 1;
		for(;;++i2){//recursion 単語のループ
			//cout << i2 << endl;
			getline(ifste,buf);
			if(buf=="") break;
			word = buf.substr(0,buf.find(" "));
			for(j=0;j<n_tags;++j){
				double x = 0;
				double max_x = 0;
				int argmax_x = 0;
				for(k=0;k<n_tags;++k){
					double a,b;
					double y;
					a = (double)(count_2gram[tags[k]+" "+tags[j]+" "]) / (counts[k]);
					b = (double)(word_tag[word+" "+tags[j]]+1) / (counts[j]+V+1);
					x += forward[i2-1][k] * a * b;
					y = viterbi[i2-1][k] * a * b;
					if(y>max_x){
						max_x = y;
						argmax_x = k;
					}
				}
				vf[j] = x;
				vv[j] = max_x;
				vb[j] = argmax_x;
			}
			//cout << vv[0] << endl;
			forward.push_back(vf);
			viterbi.push_back(vv);
			backpointer.push_back(vb);
		}
		
		//termination step
		double term_x = 0;
		double term_max_x = 0;
		int term_argmax_x = 0;
		for(j=0;j<n_tags;++j){
			double a;
			double y;
			a = (double)(count_2gram[tags[j]+" </s> "]+1) / (counts[j]);
			term_x += forward[i2-1][j] * a;
			y = viterbi[i2-1][j] * a;
			if(y>term_max_x){
				term_max_x = y;
				term_argmax_x = j;
			}
		}
		int bp = term_argmax_x;
		//output_tags.push_back(tags[term_argmax]);
		//int bp;
		for(i=0;i<i2;++i){
			output_tags.push_back(tags[bp]);
			bp = backpointer[i2-1-i][bp];
			//cout << bp << endl;
		}
		//cout << term_max_x << endl;
		
		for(i=i2-1;i>=0;--i){
			ofst << output_tags[i] << endl;
		}
		ofst << endl;
		
		/*
		for(l=0;l<i2;++l){
			for(k=0;k<n_tags;++k){
				cout << forward[l][k] << " ";
			}
			cout << endl;
		}
		*/
	}

}