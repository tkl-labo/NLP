#include<iostream>
#include<fstream>
#include<string>
#include<unordered_map>
using namespace std;

int main(){

	string in_s[2] = {"train.txt","test.txt"};
	string out_s[2] = {"train3.txt","test3.txt"};
	ofstream ofstl("tag_list.txt");
	ofstream ofswl("word_list.txt");
	unordered_map<string,int> tag_number;
	//unordered_map<string,int> word_number;
	
	int n_tags = 0;
	string tag;
	int i,j,k,l;

	for(i=0;i<2;++i){
		ifstream ifs(in_s[i]);
		ofstream ofs(out_s[i]);
		while(1){
			string buf,buf2;
			getline(ifs,buf);
			if(ifs.eof()) break;
			if(buf==""){
				ofs << endl;
				continue;
			}
			tag = buf.substr(buf.find(" ")+1, buf.rfind(" ")-buf.find(" ")-1);
			buf = buf.substr(0,buf.find(" "));
			if(tag_number[tag] == 0){
				tag_number[tag] = ++n_tags;
				ofstl << tag << endl;
				//++n_tags;
			}
			ofs << buf+" " << tag_number[tag]-1 << endl;
		}
	}
	cerr << n_tags << endl;
	
}



