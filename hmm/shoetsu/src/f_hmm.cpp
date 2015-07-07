
#include "f_hmm.h"
#include <iostream>
using namespace std;

enum Features{
  END_ADJ,
  END_NOUN,
  END_ED,
  END_ER,
  END_EST,
  OTHERS
};



F_HmmNode::F_HmmNode(const HmmKey key) : HmmNode(key){

};


void F_Hmm::SetupClassifiers(){

  auto matchSuffix = [](const string & str, const vector<string> tokens){
    for(auto it = tokens.begin(); it != tokens.end(); it++){
      int len =  it->size();
      if(str.size() >= len){
	if(str.substr(str.size() - len) == *it){
	  return true;
	}
      }
    }
    return false;
  };

  // 未知語に対する分類
  Classifiers[END_ADJ] = [&](const string& str){
    const vector<string> tokens = {
      "less", "ible", "able", "ful", "ous", "al", "an"
    };
    return matchSuffix(str, tokens);
  };

  Classifiers[END_NOUN] = [&](const string& str){
    const vector<string> tokens = {
      "eer", "ess", "ion", "age", "ty"
    };
    return matchSuffix(str, tokens);
  };

  Classifiers[END_ED] = [&](const string& str){
    const vector<string> tokens = {
      "ed"
    };
    return matchSuffix(str, tokens);
  };

  Classifiers[END_ER] = [&](const string& str){
    const vector<string> tokens = {
      "er"
    };
    return matchSuffix(str, tokens);
  };

  Classifiers[END_EST] = [&](const string& str){
    const vector<string> tokens = {
      "est"
    };
    return matchSuffix(str, tokens);
  };

  Classifiers[END_EST] = [&](const string& str){
    const vector<string> tokens = {
      "est"
    };
    return matchSuffix(str, tokens);
  };


}
F_Hmm::F_Hmm(){
  SetupClassifiers();
  cout << Classifiers[END_ADJ]("nevertheress");
  cout << Classifiers[END_ADJ]("nevertheless");
  cout << Classifiers[END_ADJ]("k");

};

