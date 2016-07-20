#include<iostream>
#include<fstream>
#include<string>
using namespace std;

int main(){

	string in_s[2] = {"train.txt","test.txt"};
	string out_s[2] = {"train2.txt","test2.txt"};
	
	int i,j,k,l;
	
	for(i=0;i<2;++i){
		ifstream ifs(in_s[i]);
		ofstream ofs(out_s[i]);
		while(1){
			string buf,buf2;
			getline(ifs,buf);
			if(ifs.eof()) break;
			buf = buf.substr(0,buf.rfind(" "));
			ofs << buf << endl;
		}
	}
	
}



