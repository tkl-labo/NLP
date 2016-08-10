#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<unordered_map>
using namespace std;

int main(){
	ifstream ifs("train3.txt");
	//ifstream ifstl("tag_list.txt");
	ofstream ofst1("tag_1gram4.txt");
	ofstream ofst2("tag_2gram4.txt");
	ofstream ofst3("tag_3gram4.txt");
	string buf;
	int i,j,k;
	const int n_tags=44;
	cerr << n_tags << endl;
	//sstream sst2, sst3;
	//ofstream ofsw("word.txt");
	//string tags[];
	//string words[];
	vector<string> all_tags;
	int tag_1gram[n_tags+2]={0};
	int tag_2gram[n_tags+2][n_tags+2]={0};
	int tag_3gram[n_tags+2][n_tags+2][n_tags+2]={0};
	
	int tag;
	vector<int> tag3={n_tags,n_tags,n_tags};
	while(1){
		string key1,key2,key3;
		getline(ifs,buf);
		if(ifs.eof()) break;
		if(buf == "") tag = n_tags+1;
		else tag = stoi(buf.substr(buf.rfind(" ")+1));
		for(i=1;i<3;++i) tag3[i-1] = tag3[i];
		tag3[2] = tag;
		tag_3gram[tag3[2]][tag3[1]][tag3[0]] += 1;
		tag_2gram[tag3[2]][tag3[1]] += 1;
		tag_1gram[tag3[2]] += 1;
		if(buf == "") tag3={n_tags,n_tags,n_tags};
	}
	int c1=0,c2=0,c3=0;
	for(i=0;i<n_tags+2;++i){
		//ofst1 << i << " " << tag_1gram[i] << endl;
		ofst1 << tag_1gram[i] << endl;
		c1 += tag_1gram[i];
		for(j=0;j<n_tags+2;++j){
			//ofst2 << i << " " << j << " " << tag_2gram[i][j] << endl;
			ofst2 << tag_2gram[i][j] << endl;
			c2 += tag_2gram[i][j];
			for(k=0;k<n_tags+2;++k){
				//if(tag_3gram[i][j][k] != 0) ofst3 << i << " " << j << " " << k << " " << tag_3gram[i][j][k] << endl;
				ofst3 << tag_3gram[i][j][k] << endl;
				c3 += tag_3gram[i][j][k];
			}
		
		}
	}
	
	cerr << c1 << endl;
	cerr << c2 << endl;
	cerr << c3 << endl;
}