
// #include "util.h"
// #include "f_hmm.h"
// #include <iostream>
// #include <cassert>
// #include <fstream>

// using namespace std;

// //========================
// //       F_HmmNode
// //========================

// F_HmmNode::F_HmmNode(const HmmKey key) : HmmNode(key){

// };

// void F_HmmNode::FeatureEmissionLearning(UnknownType type){
//   (*m_femissions)[type]++;
// }


// //========================
// //         F_Hmm
// //========================

// F_Hmm::F_Hmm(){
//   SetupClassifiers();
// };


// UnknownType F_Hmm::ClassifyUnknown(const string &str){
//   for(int type = F_CAP; type != F_OTHERS; type++){
//     if (Classifiers[type] == NULL){
//       continue; 
//     }
    
//     if (Classifiers[type](str)){
//       return (UnknownType)type; 
//     }
//   }
//   return F_OTHERS;
// }

// void F_Hmm::SetupClassifiers(){

//   auto matchSuffix = [](const string & str, const vector<string> tokens){
//     for(auto it = tokens.begin(); it != tokens.end(); it++){
//       int len =  it->size();
//       if(str.size() >= len){
// 	if(str.substr(str.size() - len) == *it){
// 	  return true;
// 	}
//       }
//     }
//     return false;
//   };

//   auto matchAll = [](const string & str, const vector<string> tokens){
//     for(auto it = tokens.begin(); it != tokens.end(); it++){
//       if(str == *it){
// 	return true;
//       }
//     }
//     return false;
//   };

//   // 未知語に対する分類
//   // ここでは各単語が一意に属するように分類する

//   Classifiers[F_CAP] = [&](const string& str){
//     assert(str.size() > 0);
//     if(str[0] >= 'A' && str[0] <= 'Z'){
//       return true;
//     }
//     return false;
//   };

//   Classifiers[F_CD] = [&](const string& str){
//     int n = str.size();
//     assert(str.size() > 0);
    
//     if(str[0] >= '0' && str[0] <= '9' && str[n-1] >= '0' && str[n-1] <= '9'){
//       return true;
//     }
//     return false;
//   };

//   Classifiers[F_END_ADJ] = [&](const string& str){
//     const vector<string> tokens = {
//       "less", "ible", "able", "ful", "ous", "al", "an"
//     };
//     return matchSuffix(str, tokens);
//   };

//   Classifiers[F_END_NOUN] = [&](const string& str){
//     const vector<string> tokens = {
//       "eer", "ess", "ion", "age", "ty"
//     };
//     return matchSuffix(str, tokens);
//   };

//   Classifiers[F_END_ED] = [&](const string& str){
//     const vector<string> tokens = {
//       "ed"
//     };
//     return matchSuffix(str, tokens);
//   };

//   Classifiers[F_END_ER] = [&](const string& str){
//     const vector<string> tokens = {
//       "er"
//     };
//     return matchSuffix(str, tokens);
//   };

//   Classifiers[F_END_EST] = [&](const string& str){
//     const vector<string> tokens = {
//       "est"
//     };
//     return matchSuffix(str, tokens);
//   };

//   Classifiers[F_END_ING] = [&](const string& str){
//     const vector<string> tokens = {
//       "ing"
//     };
//     return matchSuffix(str, tokens);
//   };
//   Classifiers[F_END_S] = [&](const string& str){
//     const vector<string> tokens = {
//       "s"
//     };
//     return matchSuffix(str, tokens);
//   };

//   Classifiers[F_VERB] = [&](const string& str){
//     const vector<string> tokens = {
//       "ate"
//     };
//     return matchSuffix(str, tokens);
//   };


// }


// void F_Hmm::Learn(const string &filename){

//   ifstream ifs(filename);
//   if(ifs.fail()){
//     cout << "failed to read text." << endl;
//     exit(1);
//   }

//   HmmNodePtr node = GetNode(START_NODE_KEY);
//   string line;

//   while(getline(ifs,line)){
//     auto strv = split(line);
 
//     if (strv.size() != 0){
//       node->TransitionLearning(strv[1]);
//       node = GetOrCreateNode(strv[1]);

//       node->EmissionLearning(strv[0]);

//       auto f_type = ClassifyUnknown(strv[0]);
//       (F_HmmNodePtr)node->FeatureEmissionLearning(f_type);
      
//       AddToVocablary(strv[0]);
//     }else{
//       node = GetNode(START_NODE_KEY);
//     }
//   }
//   exit(1);
// }


