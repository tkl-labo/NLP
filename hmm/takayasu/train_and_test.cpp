#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<unordered_map>
using namespace std;


//
//cutコマンド
//後のタグ→前のタグを読み込む、みたいな感じにするとキャッシュミスが減る!


int main(){
	int i,j,k,l;
	string buf;
	
	ifstream ifstl("tag_list.txt");
	ifstream ifst1("tag_1gram4.txt");
	ifstream ifst2("tag_2gram4.txt");
	ifstream ifst3("tag_3gram4.txt");
	ifstream ifstr("train3.txt");
	ifstream ifste("test3.txt");
	ofstream ofst("output_tagss.txt");
	ofstream ofst2("output_tags2.txt");
	const int n_tags = 44;
	int tag_1gram[n_tags+2]={0};
	int tag_2gram[n_tags+2][n_tags+2]={0};
	int tag_3gram[n_tags+2][n_tags+2][n_tags+2]={0};
	for(i=0;i<n_tags+2;++i){
		getline(ifst1,buf);
		//cout << buf << endl;
		tag_1gram[i]=stoi(buf);
		for(j=0;j<n_tags+2;++j){
			getline(ifst2,buf);
			tag_2gram[i][j]=stoi(buf);
			for(k=0;k<n_tags+2;++k){
				getline(ifst3,buf);
				tag_3gram[i][j][k]=stoi(buf);
			}
		}
	}
	ifst1.close();
	ifst2.close();
	ifst3.close();
	unordered_map<string,int>word_tag;
	unordered_map<string,int>count_words;
	unordered_map<string,int>::const_iterator itr;


	vector<int>counts;
	int V=0;
	int count;
	string word,tag,ngram;
	int n_correct=0;
	int n_wrong=0;

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
	//語彙のカウント <UNK>は含まない
	for(itr=count_words.begin();itr!=count_words.end();++itr) ++V;
	cerr << "V = " << V << endl;
	
	
	//int loop = 0;
	while(1){//文のループ
		//if(loop>0) break;
		//if(loop%100 == 0) cerr << loop << endl;
		//++loop;
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
		vector<int>input_tags;
		vector<int>output_tags;
		input_tags.push_back(stoi(buf.substr(buf.rfind(" ")+1)));
		
		
		for(j=0;j<n_tags;++j){
			double a,b;
			a = (double)(tag_2gram[j][n_tags]) / tag_1gram[n_tags+1];
			b = (double)(word_tag[word+" "+to_string(j)]+1) / (tag_1gram[j]+V+1) ;
			vf[j] = a * b;
			vv[j] = vf[j];
			vb[j] = 0;
		}
		forward.push_back(vf);
		viterbi.push_back(vv);
		backpointer.push_back(vb);
		
		int i2 = 1;
		for(;;++i2){//recursion step単語のループ
			//cout << i2 << endl;
			getline(ifste,buf);
			if(buf=="") break;
			word = buf.substr(0,buf.rfind(" "));
			input_tags.push_back(stoi(buf.substr(buf.rfind(" ")+1)));
			ofst2 << input_tags[i2-1] << endl;
			for(j=0;j<n_tags;++j){
				double x = 0;
				double max_x = 0;
				int argmax_x = 0;
				for(k=0;k<n_tags;++k){
					double a,b;
					double y;
					a = (double)(tag_2gram[j][k]) / (tag_1gram[k]);
					b = (double)(word_tag[word+" "+to_string(j)]+1) / (tag_1gram[j]+V+1);
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
			a = (double)(tag_2gram[n_tags+1][j]+1) / (tag_1gram[j]);
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
			output_tags.push_back(bp);
			bp = backpointer[i2-1-i][bp];
			//cout << bp << endl;
		}
		//
		//cout << term_max_x << endl;
		
		for(i=i2-1;i>=0;--i){
			cerr << output_tags[i] << " " << input_tags[i2-1-i] << endl;
			if(output_tags[i] == input_tags[i2-1-i]) ++n_correct;
			else ++n_wrong;
			//ofst << output_tags[i] << endl;
		}
		//ofst << endl;
	}
	cerr << n_correct << endl;
	cerr << n_correct + n_wrong << endl;
}