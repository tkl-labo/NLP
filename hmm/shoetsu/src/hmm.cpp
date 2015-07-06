

#include "hmm.h"
#include "util.h"

#include <fstream>
#include <cassert>
#include <iostream>

using namespace std;

const string START_NODE_KEY = "<S>";
const vector<string> EXCEPTION_KEYS = {",", "."};
const string END_LINE = "";

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


void Hmm::Learn(){
  string line;
  string word;
  string PoS;
  int c = 0;

  HmmNodePtr node = GetNode(START_NODE_KEY);
  while(cin >> line){

    switch(c){
    case 0:
      word = line;
      node->EmissionLearning(word);
      c++;
      break;
    case 1:
      PoS = line;
      node->TransitionLearning(PoS);
      node = GetOrCreateNode(PoS);
      c--;

      if (line == "."){ //空行はどうする？
	node = GetNode(START_NODE_KEY);
      }
      break;
    default:
      exit(1);
    }
  }
  //Show();
}


void Hmm::Show(){
  for(auto it = m_nodes->begin(); it != m_nodes->end();it++){
    auto node = it->second;


    printf("<HmmKey> : %s\n", node->GetKey());
    auto emissions = node->GetEmissions();
    for(auto it = emissions.begin(); it != emissions.end(); it++){
         cout << it->first << " " << node->GetEmissionProb(it->first)<< endl;
    }
  }
}



vector<HmmKey> Hmm::Viterbi(const vector<string> &o){
  HmmNodePtr start_node = GetOrCreateNode(START_NODE_KEY);
  int i,j,k;

  unordered_map<HmmKey, vector<HmmKey>> route_prev;
  unordered_map<HmmKey, vector<HmmKey>> route_next;
  unordered_map<HmmKey, double> prob_prev;
  unordered_map<HmmKey, double> prob_next;
 
  for(auto it = m_nodes->begin(); it != m_nodes->end(); it++){
    const HmmKey key = it->first; 
    const HmmNodePtr node = it->second;
    route_prev[key].push_back(key);
    prob_prev[key] = start_node->GetTransitionProb(key);
    prob_prev[key] *= node->GetEmissionProb(o[0]);
  }


  for(i = 1; i < o.size(); i++){
    for(auto it_next = m_nodes->begin(); it_next != m_nodes->end(); it_next++){
      HmmKey key_next = it_next->first;
      HmmNodePtr node_next = it_next->second;
      prob_next[key_next] = 0;
      double max_prob = 0;
      HmmKey max_prob_key;

      for(auto it_prev = m_nodes->begin(); it_prev != m_nodes->end(); it_prev++){
	const HmmKey key_prev = it_prev->first;
	const HmmNodePtr node_prev = it_prev->second;
	double prob = prob_prev[key_prev] * node_prev->GetTransitionProb(key_next);
	// cout << "--------------" <<endl;
	// cout << prob << endl;
	// cout << prob_prev[key_prev] <<endl;
	// cout << node_prev->GetTransitionProb(key_next) <<endl;

	if(prob > max_prob){
	  max_prob = prob;
	  max_prob_key = key_prev;
	}
	prob_next[key_next] += prob;
      }
      prob_next[key_next] *= node_next->GetEmissionProb(o[i]);

      //最も確率の高いルートを選択
      route_prev[max_prob_key].push_back(key_next);
      route_next[key_next] = route_prev[max_prob_key];
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

  
  // for(auto it = route_next[max_prob_key].begin(); it != route_next[max_prob_key].end(); it++){

  // }

  return route_next[max_prob_key];

}


double Hmm::Test(){
  double accuracy = 0;

  vector<string> test_str = split("Confidence in the pound is widely expected to take another sharp dive");
  vector<HmmKey> result = Viterbi(test_str);
  
  for (auto it = result.begin(); it != result.end(); it++)
     cout << *it <<endl;

  //Show();
  return accuracy;
}
