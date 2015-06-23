

#include "n_gram.h"


using namespace std;

const wstring EOS = L"";
const wstring EOSSTRING = L"EOS";


static NGramKey_t StartNodeKey(int n){
  NGramKey_t strv(n, EOS);
  return strv;
}

static void OutputKey(NGramNodePtr_t node){
  wcout << "Key : ( ";
  for(auto str: node->GetKey()){
    wcout <<str << " , ";
  }
  wcout << ")" << endl;

}


//================================
//        NGramNode
//================================

NGramNode::NGramNode(NGramKey_t strv){
  m_total = 0;
  m_freq = make_shared<NGramFreq_t>(); 
  m_key = make_shared<NGramKey_t>(strv); 
}

void NGramNode::AddFreq(const wstring &str){
  m_total++;
  (*m_freq)[str]++;
 
}


float NGramNode::OutputProb(const wstring& str){
  float prob = this->GetProb(str); 
  wcout << "Prob[" 
	<< str 
	<< " | (" ; 
  for(auto str: this->GetKey()){
    wcout <<str << ", ";
  }
  wcout << ")] : " 
	<< prob <<endl;
  return prob;
}


//================================
//          NGram
//================================

NGram::NGram(const int n){
  N = n;
  m_map = make_shared<NGramMap_t>();

  GetOrCreateNode(StartNodeKey(N));
}


NGramNodePtr_t NGram::GetOrCreateNode(NGramKey_t strv){
  auto it = m_map->find(strv);
  if(it == m_map->end()){
    //ノードが存在しない場合は作成
    NGramNodePtr_t node = make_shared<NGramNode>(strv); 
    (*m_map)[strv] = node;
    return node;
  }else{
    return it->second;
  }
}


NGramNodePtr_t NGram::GetStartNode(){
  NGramKey_t strv = StartNodeKey(N);
  return GetOrCreateNode(strv);
}


void NGram::Learn(){
  //1つのEOSまでを1シークエンスとする
  wstring str;
  NGramKey_t strv;
  while( wcin >> str ){ 
    strv.push_back(str);
    if (str == EOSSTRING){
      Add(strv);
      wcout << L"EOS-------------" <<endl;
      strv.clear();
    }
  }
  
};

void NGram::Add(NGramKey_t source){
  NGramKey_t strv = StartNodeKey(N);

  NGramNodePtr_t node;
  for(auto it = source.begin(); it < source.end(); it++){
    node = GetOrCreateNode(strv);
    node->AddFreq(*it);
    strv.erase(strv.begin());
    strv.push_back(*it);
    node->OutputProb(*it);
  }
  node = GetOrCreateNode(strv);
  node->AddFreq(EOS);
  
}

wstring NGram::CreateRandomSentence(){
  
}

