#include<iostream>
#include<fstream>
#include<string>
#include<vector>
using namespace std;


int main(){
	ifstream ifs("train2.txt");
	ofstream ofst2("tag_2gram.txt");
	ofstream ofst3("tag_3gram.txt");
	//ofstream ofsw("word.txt");
	//string tags[];
	//string words[];
	int i;
	//unordered_map tag_to_tag<string,string>
	//unordered_map 2gram_to_tag<string,string>
	string buf,word,tag;
	vector<string> tag3={"<s>","<s>","<s>"};
	while(1){
		getline(ifs,buf);
		if(ifs.eof()) break;
		if(buf == "") tag = "</s>";
		else tag = buf.substr(buf.rfind(" ")+1);
		for(i=1;i<3;++i) tag3[i-1] = tag3[i];
		tag3[2] = tag;
		for(i=0;i<3;++i) ofst3 << tag3[i] << " ";
		ofst3 << endl;
		for(i=1;i<3;++i) ofst2 << tag3[i] << " ";
		ofst2 << endl;
		if(buf == "") tag3={"<s>","<s>","<s>"};
		
		//if(ifs.eof()) break;
		
		
		
		
	}
	
	
	


}