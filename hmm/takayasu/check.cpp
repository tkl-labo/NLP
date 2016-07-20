#include<iostream>
#include<fstream>
#include<string>
using namespace std;

int main(){
	ifstream ifs1("test2.txt");
	ifstream ifs2("output_tag.txt");
	int n_correct = 0;
	int n_wrong = 0;
	string buf1,buf2;
	while(1){
		getline(ifs1,buf1);
		getline(ifs2,buf2);
		if(ifs1.eof() || ifs2.eof()) break;
		if(buf1=="" && buf2 =="")continue;
		if(buf1.substr(buf1.find(" ")+1) == buf2) ++n_correct;
		else ++n_wrong;
	}
	cout << n_correct << endl;
	cout << n_wrong << endl;
	cout << (double)n_correct/(n_correct+n_wrong) << endl;
}