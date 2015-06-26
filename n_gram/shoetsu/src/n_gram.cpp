

#include "n_gram.h"
#include "util.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <omp.h>

#include <cmath>

/*

#pragma omp parallel sections

*/

using namespace std;


const string START = "<S>";                  // NGramのノードで用いるSTART記号 
const string EOS = "</S>";                   // NGramのノードで用いるEOS記号 
const string CORPUS_EOS_STRING = "EOS"; // コーパス中のEOS記号 
const string HEAD = "";    // ダンプ時のノード毎の区切り

const double LLimit = 1.0e-307; //ある文章の出力確率の下限(0になるとperplexityがinfになるので)

static NGramKey_t StartNodeKey(int n){
  NGramKey_t key(n, START);
  return key;
}


static void OutputKey(NGramNodePtr_t node){
   cout << "Key : (";
   auto key = node->GetKey();
   for(auto str = key.begin(); str < key.end()-1; str++){
     cout << *str << ", ";
   }
   cout << key.back()
        << ")" << endl;
}


//================================
//        NGramNode
//================================

NGramNode::NGramNode(const NGramKey_t &key){
  m_total = 0;
  m_freq = make_unique<NGramFreq_t>(); 
  m_key = make_unique<NGramKey_t>(key); 
}

void NGramNode::AddFreq(const string &word){
  m_total++;
  (*m_freq)[word]++;
}

void NGramNode::SetFreq(const string &word, const int freq){
  m_total+= freq;
  (*m_freq)[word]+= freq;
}



//================================
//          NGram
//================================

NGram::NGram(const int n){
  N = n-1;
  m_map = make_unique<NGramMap_t>();
  m_vocablary = make_unique<NGramVocablary_t>();

  GetOrCreateNode(StartNodeKey(N));
}



NGramNodePtr_t NGram::GetNode(const NGramKey_t &key) const{
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
    if (str == CORPUS_EOS_STRING){
      str = EOS;  // コーパス中のEOS記号から変換
      Add(strv);
      strv.clear();
    }else{
      strv.push_back(str);
    }
    AddToVocablary(str);
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
  NGramKey_t start_key = StartNodeKey(N);
  auto start_node = GetOrCreateNode(start_key);
  string str = Transit(start_node);

  //start_node->OutputProb();
  
  return str;
}


string NGram::Transit(NGramNodePtr_t node){
  string output_str = EOS;
  NGramNodePtr_t next_node;
  double r = (double)rand() / (double)RAND_MAX;
  float cum_prob = 0;

  for(auto freq :node->GetFreq()){
    output_str = freq.first;
    cum_prob += GetProb(node, output_str);
    if(r <= cum_prob){
      //cout << output_str << ": " << GetProb(node, output_str) <<endl;
      OutputProb(node, output_str);
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

void NGram::Save(const string & filename) const{
  ofstream ofs(filename);
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
  ifstream ifs(filename);
  if(ifs.fail()){
    cout << "Failed to load n_gram data" << endl;
    exit(1);
  }
  string line;
  NGramNodePtr_t node; 
  NGramKey_t key;
  string word;
  int freq;

  double t0,t1,t2,t3,ta,tb,tc;
  while(getline(ifs, line)){
    if(line == HEAD){
      getline(ifs, line);
      t0 = cur_time();
      key = split(line);
      t1 = cur_time();
      node = GetOrCreateNode(key);
      t2 = cur_time();
      t3 = cur_time();

    }else{
      t0 = cur_time();
      NGramKey_t wf = split(line);
      t1 = cur_time(); 
      word = wf[0];
      freq = stoi(wf[1]);
      node->SetFreq(word, freq);
      t2 = cur_time();
      AddToVocablary(word);
      t3 = cur_time();
    }
    ta += t1-t0;
    tb += t2-t1;
    tc += t3-t2;
  }
  printf( "Elapsed Time(Load-split): %.5f sec\n",ta);
  printf( "Elapsed Time(Load-node) : %.5f sec\n",tb);
  printf( "Elapsed Time(Load-addtovocab) : %.5f sec\n",tc);
}

double NGram::OutputProb(NGramNodePtr_t node, const string& str) const{
  float prob = GetProb(node, str);
  cout << "Prob[" 
       << str 
       << " | (" ; 
  NGramKey_t key = node->GetKey();
  for(auto str = key.begin(); str < key.end()-1; str++){
    cout << *str << ", ";
  }
  cout << key.back()
       << ")] : " 
       << prob <<endl;
  return prob;
}

double NGram::OutputProb(NGramNodePtr_t node) const{
  for(auto it: node->GetFreq()){
    OutputProb(node ,it.first);
  }
}


double NGram::SequenceProb(const NGramKey_t & strv) const{
  double prob = 1.0;
  NGramKey_t key = StartNodeKey(N);
  NGramNodePtr_t node = GetNode(key);

  for(auto str: strv){
    prob *= GetProb(node, str);
    if (prob < LLimit){
      return LLimit;
    }
    key.erase(key.begin());
    key.push_back(str);
    node = GetNode(key);
  }

  return prob;
}

double NGram::PerplexityTest(){
  //1つのEOSまでを1シークエンスとする
  string str;
  NGramKey_t strv;
  int n = 0;
  double perplexity = 1.0;

  vector<double> seqprob_list; //全部のseqprobをかけるとdoubleの範囲を超えちゃうので
  double t = 0;
  while( cin >> str ){ 
    if (str == CORPUS_EOS_STRING){
      str = EOS;  // コーパス中のEOS記号から変換
    }
    strv.push_back(str);

    if(str == EOS){
      double prob = SequenceProb(strv);
      seqprob_list.push_back(prob);
      n += strv.size();
      strv.clear();
    }
  }
  // 各Sequence毎に計算したProbを掛けあわせる
  for(auto prob: seqprob_list){
    perplexity *= pow(prob , -1.0 / n);
  }

  cout << "Perplexity      : " << perplexity << endl;
  return perplexity;
};



//================================
//     LaplaceSmoothed-NGram
//================================


LaplaceSmoothedNGram::LaplaceSmoothedNGram(const int n) : NGram(n){}

double LaplaceSmoothedNGram::GetProb(NGramNodePtr_t node,const string &str) const
{
   return (double)(node->GetFreq(str) + 1) / (double)(node->GetTotal() + m_vocablary->size() + 1);
}


string LaplaceSmoothedNGram::Transit(NGramNodePtr_t node){
  string output_str = EOS;
  NGramNodePtr_t next_node;
  double r = (double)rand() / (double)RAND_MAX;
  float cum_prob = 0;
  NGramVocablary_t vocablary = this->GetVocablary();

  // NGramに記録されているものを優先的に
  for(auto freq : node->GetFreq()){
    output_str = freq.first;
    vocablary.erase(output_str); 
    cum_prob += GetProb(node, output_str);
    if(r <= cum_prob){
      OutputProb(node, output_str);
      NGramKey_t key = node->GetKey();
      key.erase(key.begin());
      key.push_back(output_str);
      if(output_str == EOS){
	return "";
      }
      next_node = GetNode(key);
      return output_str + Transit(next_node);
    }
  }
  
  for(auto str : vocablary){
    output_str = str;
    cum_prob += GetProb(node, output_str);
    if(r <= cum_prob){
      OutputProb(node, output_str);
      NGramKey_t key = node->GetKey();
      key.erase(key.begin());
      key.push_back(output_str);
      if(output_str == EOS){
	return "";
      }
      next_node = GetNode(key);
      return output_str + Transit(next_node);

    }
  }

  return output_str + Transit(next_node);
}
