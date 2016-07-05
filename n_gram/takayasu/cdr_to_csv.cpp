#include <iostream>
#include <string>

using namespace std;


int main(int argc, char** argv){
	string strbuf;
	string wordbuf;
	while(getline(cin,strbuf)){
		if(strbuf == "") continue;
		if(strbuf == "EOS") wordbuf = "</s>\n";
		else{
			string::size_type index = strbuf.find("\t");
			if(index == string::npos) wordbuf = strbuf + " ";
			else wordbuf = strbuf.substr(0,index) + " ";
		}
		cout << wordbuf;
	}
	
	return 0;

}