#include <iostream>
#include <string>

using namespace std;


int main(int argc, char** argv){
	string strbuf;
	string wordbuf;
	while(getline(cin,strbuf)){
		if(strbuf == "") continue;
		if(strbuf == "EOS") wordbuf = "</s>\t\n";
		else{
			string::size_type index = strbuf.find("\t");
			if(index == string::npos) wordbuf = strbuf + "\t";
			else wordbuf = strbuf.substr(0,index) + "\t";
		}
		cout << wordbuf;
	}
	
	return 0;

}