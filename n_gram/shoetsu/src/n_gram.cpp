

#include "n_gram.h"

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

void NGramNode::AddFreq(const WordID_t &word){
  m_total++;
  (*m_freq)[word]++;
}

void NGramNode::SetFreq(const WordID_t &word, const int freq){
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
 
  AddToVocablary(START);
  AddToVocablary(EOS);

  GetOrCreateNode(StartNodeKey(N));
}


NGramKey_t NGram::StartNodeKey(const int n) const{
  auto start_id = m_vocablary->str2id(START);
  NGramKey_t key(n, start_id);
  return key;
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



WordID_t NGram::AddToVocablary(const string &str){
  auto id = m_vocablary->AddStr(str);
  return id;
}


void NGram::Learn(){
  //1つのEOSまでを1シークエンスとする
  string str;
  NGramKey_t key;
  int c = 0;
  while( cin >> str ){ 
    c++;

    if (str == CORPUS_EOS_STRING){
      str = EOS;  // コーパス中のEOS記号から変換
      AddToVocablary(str);
      Add(key);
      key.clear();
    }else{
      auto id = AddToVocablary(str);
      key.push_back(id);
    }
  }
  cout << "Learned from: " << c << " words" <<endl;
  cout << "Vocablary   : " << m_vocablary->GetCount() << " words" << endl;
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
  auto eos_id = m_vocablary->str2id(EOS);
  node->AddFreq(eos_id);

}

string NGram::CreateRandomSentence(const string & keywords){
  
  auto strv = split(keywords);

  string str;

  if(strv.size() > 0){

  }else{
    NGramKey_t start_key = StartNodeKey(N);
    auto start_node = GetOrCreateNode(start_key);
    str = ForwardTransit(start_node);
  }

  return str;
}


string NGram::ForwardTransit(NGramNodePtr_t node){
  string output_str = EOS;
  NGramNodePtr_t next_node;

  double r = Util::Random();
  float cum_prob = 0;

  for(auto freq :node->GetFreq()){
    auto id = freq.first;
    cum_prob += GetProb(node, id);
    if(r <= cum_prob){
      OutputProb(node, id);
      NGramKey_t key = node->GetKey();
      key.erase(key.begin());
      key.push_back(id);
      output_str = m_vocablary->id2str(id);
      if(output_str == EOS || output_str == START){
	return "";
      }
      next_node = GetNode(key);
    
      break;
    }
  }

  auto s = node->GetFreq().size();
  cout << s <<endl;
  if(s == 0){
    for(auto id : node->GetKey()){
      cout << m_vocablary->id2str(id) << endl;
    }
  }
  return output_str + ForwardTransit(next_node);
}

// string NGram::BackwardTransit(NGramNodePtr_t node){
//   string output_str = START;
//   NGramNodePtr_t prev_node;

//   double r = Util::Random();
//   double max_prob = 0;


//   for(auto word : GetVocablary()){
//     NGramKey_t key = node->GetKey();
//     key.erase(key.begin());
//     key.insert(key.begin(), word);
//     prev_node = GetNode(key);
//     if(max_prob < GetProb(prev_node, word))
//     max_prob = GetProb(node, word);
//   }

//   if(output_str == EOS || output_str == START){
//     return "";
//   }
//   return  BackwardTransit(prev_node) + output_str;
// }



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
      ofs << m_vocablary->id2str(key) << " ";
    }
    ofs << endl;
    for(auto freq: node->GetFreq()){
      ofs << m_vocablary->id2str(freq.first) << " " << freq.second << endl;
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
  vector<string> strv;
  string word;
  int freq;

  double t0,t1,t2,t3,ta,tb,tc;
  while(getline(ifs, line)){
    if(line == HEAD){
      getline(ifs, line);
      strv = split(line);
      for(auto str: strv){
	AddToVocablary(str);
      }
      NGramKey_t key = StrvToNGramKey(strv);
      node = GetOrCreateNode(key);

    }else{
      vector<string> wf = split(line);
      word = wf[0];
      freq = stoi(wf[1]);
      auto id = AddToVocablary(word);
      node->SetFreq(id, freq);
    }
  }
  //printf( "Elapsed Time(Load-split): %.5f sec\n",ta);
  //printf( "Elapsed Time(Load-node) : %.5f sec\n",tb);
  //printf( "Elapsed Time(Load-addtovocab) : %.5f sec\n",tc);
}

double NGram::OutputProb(NGramNodePtr_t node, const WordID_t& id) const{
  float prob = GetProb(node, id);
  cout << "Prob[" 
       << m_vocablary->id2str(id) 
       << " | (" ; 
  NGramKey_t key = node->GetKey();
  for(auto id = key.begin(); id < key.end()-1; id++){
    cout << m_vocablary->id2str(*id) << ", ";
  }
  cout <<  m_vocablary->id2str(key.back())
       << ")] : " 
       << prob <<endl;
  return prob;
}

double NGram::OutputProb(NGramNodePtr_t node) const{
  for(auto it: node->GetFreq()){
    OutputProb(node ,it.first);
  }
}


NGramKey_t NGram::StrvToNGramKey(const std::vector<std::string>& strv){
  NGramKey_t key;
  for(auto it = strv.begin(); it != strv.end(); it++){
    auto id = m_vocablary->str2id(*it);
    key.push_back(id);
  }
  return key;
}

vector<string> NGram::NGramKeyToStrv(const NGramKey_t & key){
  vector<string> strv;
  for(auto it = key.begin(); it != key.end(); it++){
    auto str = m_vocablary->id2str(*it);
    strv.push_back(str);
  }
  return strv;
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
  vector<string> strv;
  NGramKey_t key;
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
      auto key = StrvToNGramKey(strv);
      double prob = SequenceProb(key);
      seqprob_list.push_back(prob);
      n += strv.size();
      strv.clear();
    }
  }
  // 各Sequence毎に計算したProbを掛けあわせる
  for(auto prob: seqprob_list){
    perplexity *= pow(prob , -1.0 / n);
  }

  return perplexity;
};



//================================
//     LaplaceSmoothed-NGram
//================================


LaplaceSmoothedNGram::LaplaceSmoothedNGram(const int n) : NGram(n){}

double LaplaceSmoothedNGram::GetProb(NGramNodePtr_t node,const WordID_t &str) const
{
  const float SMOOTH = 0.1;
  return (double)(node->GetFreq(str) + SMOOTH) / (double)(node->GetTotal() + m_vocablary->GetCount() * SMOOTH+ 1);
}


// string LaplaceSmoothedNGram::ForwardTransit(NGramNodePtr_t node){
//   string output_str = EOS;
//   NGramNodePtr_t next_node;
//   double r = (double)rand() / (double)RAND_MAX;
//   float cum_prob = 0;
//   NGramVocablary_t vocablary = this->GetVocablary();

//   // NGramに記録されているものを優先的に
//   for(auto freq : node->GetFreq()){
//     output_str = freq.first;
//     vocablary.erase(output_str); 
//     cum_prob += GetProb(node, output_str);
//     if(r <= cum_prob){
//       OutputProb(node, output_str);
//       NGramKey_t key = node->GetKey();
//       key.erase(key.begin());
//       key.push_back(output_str);
//       if(output_str == EOS){
// 	return "";
//       }
//       next_node = GetNode(key);
//       return output_str + ForwardTransit(next_node);
//     }
//   }
  
//   for(auto str : vocablary){
//     output_str = str;
//     cum_prob += GetProb(node, output_str);
//     if(r <= cum_prob){
//       OutputProb(node, output_str);
//       NGramKey_t key = node->GetKey();
//       key.erase(key.begin());
//       key.push_back(output_str);
//       if(output_str == EOS){
// 	return "";
//       }
//       next_node = GetNode(key);
//       return output_str + ForwardTransit(next_node);

//     }
//   }

//   return output_str + ForwardTransit(next_node);
// }
