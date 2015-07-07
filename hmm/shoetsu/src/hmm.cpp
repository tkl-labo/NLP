

#include "hmm.h"
#include "util.h"

#include <fstream>
#include <cassert>
#include <iostream>
#include <iomanip>

using namespace std;

const string START_NODE_KEY = "<S>";
const vector<string> EXCEPTION_KEYS = {",", "."};
const string END_LINE = "";


//========================
//   Minimal Functions
//========================
void ShowDetail(const vector<string>& sentence, const vector<HmmKey>& pos_answers, const vector<HmmKey>& pos_results, const vector<bool> &unknown_flags)
{


  for(int i = 0; i < sentence.size(); i++){

    cout << setw(15) << sentence[i] << " " 
	 << setw(4)  << pos_answers[i] << " ";

    //未知語のチェック 
    if (unknown_flags[i] == true){

      if(pos_answers[i] == pos_results[i]){
	cout << setw(4)  << pos_results[i]
	     << "   Unknown"
	     << endl;
      }else{
	cout << "\x1b[31m" 
	     << setw(4) << pos_results[i] 
	     << "   Unknown" 
	     << "\x1b[0m"  
	     << endl;
      }

    }else{

      if(pos_answers[i] == pos_results[i]){
	cout << setw(4)  << pos_results[i] << endl;
      }else{
	cout << "\x1b[31m" << setw(4) << pos_results[i] << "\x1b[0m" <<endl;
      }

    }

  }
  cout << endl;
}





//========================
//       HmmNode
//========================

HmmNode::HmmNode(const HmmKey key) : m_key(key){
  m_transitions_count = 0;
  m_emissions_count = 0;
  m_transitions = make_unique<Transitions>();
  m_emissions = make_unique<Emissions>();
  
}

void HmmNode::EmissionLearning(const string &word){
  m_emissions_count++;
  (*m_emissions)[word]++;
}

void HmmNode::TransitionLearning(const HmmKey &key){
  m_transitions_count++;
  (*m_transitions)[key]++;
}



//========================
//         Hmm
//========================

Hmm::Hmm(){
  m_nodes = make_unique<HmmNodes>();
  m_vocablary = make_unique<Vocablary>();
  CreateNode(START_NODE_KEY);
}

HmmNodePtr Hmm::GetOrCreateNode(const HmmKey &key){
  auto node = m_nodes->find(key);
  if (node == m_nodes->end()){
    (*m_nodes)[key] = CreateNode(key);
  }
  return (*m_nodes)[key];
}

HmmNodePtr Hmm::CreateNode(const HmmKey &key){
  assert(m_nodes->find(key) == m_nodes->end());
  (*m_nodes)[key] = make_shared<HmmNode>(key);
  return (*m_nodes)[key];
}


void Hmm::Learn(const string &filename){

  ifstream ifs(filename);
  if(ifs.fail()){
    cout << "failed to read text." << endl;
    exit(1);
  }

  HmmNodePtr node = GetNode(START_NODE_KEY);
  string line;

  while(getline(ifs,line)){
    auto strv = split(line);
 
    if (strv.size() != 0){
      node->TransitionLearning(strv[1]);
      node = GetOrCreateNode(strv[1]);

      node->EmissionLearning(strv[0]);
      AddToVocablary(strv[0]);
    }else{
      node = GetNode(START_NODE_KEY);
    }
  }
  
  //Show(GetNode(START_NODE_KEY), "transition");
}



void Hmm::Show(HmmNodePtr node, const string &type){
  
  cout << "---- [HmmKey] : " << node->GetKey() << " ----" << endl;
  if (type == "emission"){
    auto emissions = node->GetEmissions();
    for(auto it = emissions.begin(); it != emissions.end(); it++){
      cout << it->first << " " << GetEmissionProb(it->first, node)<< endl;
    }
  }else if (type == "transition"){
    auto transitions = node->GetTransitions();
    for(auto it = transitions.begin(); it != transitions.end(); it++){
      cout << it->first << " " << GetTransitionProb(it->first, node)<< endl;
    }
  }
}



vector<HmmKey> Hmm::Viterbi(const vector<string> &o){
  int i,j,k;

  unordered_map<HmmKey, vector<HmmKey>> route_prev;
  unordered_map<HmmKey, vector<HmmKey>> route_next;
  unordered_map<HmmKey, double> prob_prev;
  unordered_map<HmmKey, double> prob_next;
 

  // from start node
  HmmNodePtr start_node = GetOrCreateNode(START_NODE_KEY);
 
 for(auto it = m_nodes->begin(); it != m_nodes->end(); it++){
    const HmmKey key = it->first; 
    const HmmNodePtr node = it->second;
    route_prev[key].push_back(key);
    prob_prev[key] = GetTransitionProb(key, start_node);
    prob_prev[key] *= GetEmissionProb(o[0], node);
  }

  for(i = 1; i < o.size(); i++){
    for(auto it_next = m_nodes->begin(); it_next != m_nodes->end(); it_next++){
      HmmKey key_next = it_next->first;
      HmmNodePtr node_next = it_next->second;
      prob_next[key_next] = 0;
      double max_prob = 0;
      HmmKey max_prob_key;

      //1つ前のイテレーションにおける各ノードからの遷移確率を合計 
      for(auto it_prev = m_nodes->begin(); it_prev != m_nodes->end(); it_prev++){
	const HmmKey key_prev = it_prev->first;
	const HmmNodePtr node_prev = it_prev->second;
	double prob = prob_prev[key_prev] * GetTransitionProb(key_next, node_prev);
	if(prob > max_prob){
	  max_prob = prob;
	  max_prob_key = key_prev;
	}
	prob_next[key_next] += prob;
      }

      prob_next[key_next] *= GetEmissionProb(o[i], node_next);
      
      //最も確率の高いルートを選択
      route_next[key_next] = route_prev[max_prob_key];
      route_next[key_next].push_back(key_next);
    }

    prob_prev = prob_next;
    route_prev = route_next;
  }

  HmmKey max_prob_key;
  double max_prob = 0;

  for(auto it = m_nodes->begin(); it != m_nodes->end(); it++){
    const HmmKey key= it->first; 
    if (prob_next[key] > max_prob){
      max_prob = prob_next[key];
      max_prob_key = key;
    }
  }

  return route_next[max_prob_key];

}


double Hmm::Test(const string & filename){

  ifstream ifs(filename);
  if(ifs.fail()){
    cout << "failed to read text." << endl;
    exit(1);
  }
  string line;
  vector<string> sentence;
  vector<HmmKey> pos_answers;

  int total_word_count = 0;
  int right_word_count = 0;

  //shared_ptr<unordered_set<string>> unk_words = make_shared<unordered_set<string>>();
  vector<bool> unknown_flags;
  int unknown_count =0;

  while(getline(ifs,line)){
    auto strv = split(line);
    
    if (strv.size() != 0){
      total_word_count++;
      if (m_vocablary->find(strv[0]) == m_vocablary->end()){
	unknown_count++;
	unknown_flags.push_back(true);
      }else{
	unknown_flags.push_back(false);
      }

      sentence.push_back(strv[0]);
      pos_answers.push_back(strv[1]);
    }else{
      bool mistake_flag = false;
      auto pos_results = Viterbi(sentence);
     
      for(int i = 0; i < pos_results.size(); i++){
	if(pos_results[i] == pos_answers[i]){
	  right_word_count++;
	}else{
	  mistake_flag = true;
	}
      }
#ifdef DETAIL
      if (mistake_flag == true){
	ShowDetail(sentence, pos_answers, pos_results, unknown_flags);
      }
#endif
      sentence.clear();
      pos_answers.clear();
      unknown_flags.clear();
	
    }
  }


  cout << unknown_count << " unknown words found" << endl;

  return (double)right_word_count / (double)(total_word_count) * 100.0;
  

}



