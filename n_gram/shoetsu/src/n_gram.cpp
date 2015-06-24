

#include "n_gram.h"
#include "util.h"

#include <cassert>
#include <iostream>
#include <fstream>

using namespace std;


const string START = "<S>";                  // NGramのノードで用いるSTART記号 
const string EOS = "</S>";                   // NGramのノードで用いるEOS記号 
const string CORPUS_EOS_STRING = "EOS"; // コーパス中のEOS記号 
const string HEAD = "-------------";    // ダンプ時のノード毎の区切り


static NGramKey_t StartNodeKey(int n){
  NGramKey_t key(n, START);
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

NGramNode::NGramNode(const NGramKey_t &key){
  m_total = 0;
  m_freq = make_shared<NGramFreq_t>(); 
  m_key = make_shared<NGramKey_t>(key); 
}

void NGramNode::AddFreq(const string &word){
  m_total++;
  (*m_freq)[word]++;
}

void NGramNode::SetFreq(const string &word, const int freq){
  m_total+= freq;
  (*m_freq)[word]+= freq;
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
  N = n-1;
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
    if (str == CORPUS_EOS_STRING){
      str = EOS;  // コーパス中のEOS記号から変換
      strv.push_back(str);
      Add(strv);
      strv.clear();
    }else{
      strv.push_back(str);
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


//string NGram::Transit(NGramNodePtr_t node, (float)(*Prob)(NGramNodePtr_t node_,const string &str)){
string NGram::Transit(NGramNodePtr_t node){
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
      if(output_str == EOS){
	return "";
      }
      next_node = GetNode(key);
      break;
    }
  }

  return output_str + Transit(next_node);
}

void NGram::Save(const string & filename){
  ofstream ofs("data/"+filename);
  if(ofs.fail()){
    cout << "Failed to save n_gram data" << endl;
    return;
  }

  for(auto m: this->GetMap()){
    //各ノード毎のkey, count, freqを保存 
    auto node = m.second;
    ofs << HEAD << endl;
    for(auto key: node->GetKey()){
      ofs << key << " ";
    }
    ofs << endl;
    for(auto freq: node->GetFreq()){
      ofs << freq.first << " " << freq.second << endl;
    }
  }

}



//-------------
// key1 key2 ..
// word1 freq1
//     :
//-------------
//     :

void NGram::Load(const string & filename){
  ifstream ifs("data/" + filename);
  if(ifs.fail()){
    cout << "Failed to load n_gram data" << endl;
    return;
  }
  string line;
  NGramNodePtr_t node; 
  NGramKey_t key;
  string word;
  int freq;
  while(getline(ifs, line)){
    if(line == HEAD){
      getline(ifs, line);
      key = split(line);
      node = GetOrCreateNode(key);
    }else{
      auto wf = split(line);
      word = wf[0];
      freq = stoi(wf[1]);
      node->SetFreq(word, freq);
      
    }
  }
}


