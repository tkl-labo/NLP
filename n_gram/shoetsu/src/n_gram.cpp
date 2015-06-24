

#include "n_gram.h"
#include <cassert>
#include <iostream>

using namespace std;

const string EOS = "";          // 出力・NGramのノードで用いるEOS 
const string CORPUS_EOS_STRING = "EOS"; // コーパス中のEOS記号 


static NGramKey_t StartNodeKey(int n){
  NGramKey_t key(n, EOS);
  return key;
}

static void OutputKey(NGramNodePtr_t node){
  cout << "Key : ( ";
  for(auto str: node->GetKey()){
    cout <<str << " , ";
  }
  cout << ")" << endl;

}


//================================
//        NGramNode
//================================

NGramNode::NGramNode(const NGramKey_t &strv){
  m_total = 0;
  m_freq = make_shared<NGramFreq_t>(); 
  m_key = make_shared<NGramKey_t>(strv); 
}

void NGramNode::AddFreq(const string &str){
  m_total++;
  (*m_freq)[str]++;
}


float NGramNode::OutputProb(const string& str){
  float prob = (float)(*m_freq)[str] / (float)m_total; 
  cout << "Prob[" 
	<< str 
	<< " | (" ; 
  for(auto str: this->GetKey()){
    cout <<str << ", ";
  }
  cout << ")] : " 
	<< prob <<endl;
  return prob;
}

float NGramNode::OutputProb(){
  for(auto it: GetFreq()){
    OutputProb(it.first);
  }
}

//================================
//          NGram
//================================

NGram::NGram(const int n){
  N = n;
  m_map = make_shared<NGramMap_t>();
  m_vocablary = make_unique<NGramVocablary_t>();

  GetOrCreateNode(StartNodeKey(N));
}



NGramNodePtr_t NGram::GetNode(const NGramKey_t &key){
  auto it = m_map->find(key);
  if(it == m_map->end()){
    //ノードが存在しない場合は空のノードを返す
    NGramNodePtr_t node = make_shared<NGramNode>(key); 
    return node;
  }else{
    return it->second;
  }
  return it->second;
}


NGramNodePtr_t NGram::GetOrCreateNode(const NGramKey_t &key){
  auto it = m_map->find(key);
  if(it == m_map->end()){
    //ノードが存在しない場合は作成して登録
    NGramNodePtr_t node = make_shared<NGramNode>(key); 
    (*m_map)[key] = node;
    return node;
  }else{
    return it->second;
  }
}


NGramNodePtr_t NGram::GetStartNode(){
  NGramKey_t strv = StartNodeKey(N);
  return GetOrCreateNode(strv);
}


void NGram::AddToVocablary(string str){
  m_vocablary->insert(str);
}


void NGram::Learn(){
  //1つのEOSまでを1シークエンスとする
  string str;
  NGramKey_t strv;
  int c = 0;
  while( cin >> str ){ 
    c++;
    AddToVocablary(str);
    strv.push_back(str);
    if (str == CORPUS_EOS_STRING){
      Add(strv);
      strv.clear();
    }
  }
  cout << "Learned from: " << c << " words" <<endl;
  cout << "Vocablary   : " << m_vocablary->size() << " words" << endl;
};


//１つの文の中の単語列によるN_Gramを追加する
void NGram::Add(const NGramKey_t &source){
  NGramKey_t key = StartNodeKey(N);
  NGramNodePtr_t node;

  for(auto it = source.begin(); it < source.end(); it++){
    node = GetOrCreateNode(key);
    node->AddFreq(*it);
    key.erase(key.begin());
    key.push_back(*it);
  }

  //最後にEOSへ飛ばす 
  node = GetOrCreateNode(key);
  node->AddFreq(EOS);

}

string NGram::CreateRandomSentence(){
  NGramKey_t strv = StartNodeKey(N);
  auto start_node = GetStartNode();
  string str = Transit(start_node);

  //start_node->OutputProb();
  
  return str;
}


string NGram::Transit(NGramNodePtr_t node, (float)(*Prob)(NGramNodePtr_t node_,const string &str)){
  string output_str = EOS;
  NGramNodePtr_t next_node;
  double r = (double)rand() / (double)RAND_MAX;
  float cum_prob = 0;

  for(auto freq :node->GetFreq()){
    cum_prob += node->GetProb(freq.first);
    if(r <= cum_prob){
      output_str = freq.first;
      NGramKey_t key = node->GetKey();
      key.erase(key.begin());
      key.push_back(output_str);
      if(output_str == CORPUS_EOS_STRING){
	return EOS; // 末尾EOSのノードは存在しない
      }
      next_node = GetNode(key);
      break;
    }
  }

  return output_str + Transit(next_node);
}
