

#ifndef HMM_H_INCLUDED
#define HMM_H_INCLUDED

#include <memory>
#include <vector>
#include <string.h>

#include <unordered_map>
#include <unordered_set>


const std::string START_NODE_KEY = "<S>";
const std::vector<std::string> EXCEPTION_KEYS = {",", "."};
const std::string END_LINE = "";


typedef std::string HmmKey;
typedef std::unordered_map<HmmKey, int> Transitions;
typedef std::unordered_map<std::string, int> Emissions;

#include <iostream>
class HmmNode{
 protected:
  std::unique_ptr<Transitions> m_transitions;
  std::unique_ptr<Emissions> m_emissions;
  int m_transitions_count;
  int m_emissions_count;

  const HmmKey m_key;
 public:
  HmmNode(const HmmKey key);
  virtual ~HmmNode() = default;

  HmmKey GetKey() { return m_key;};
  void EmissionLearning(const std::string &);
  void TransitionLearning(const HmmKey &);

  //ラプラススムージングを使用
  inline double GetEmissionProb(const std::string &word, const int n_vocab){
    return (double)((*m_emissions)[word] + 1) / (double)(m_emissions_count + n_vocab + 1);
  }
  inline double GetTransitionProb(const HmmKey & key){
    return (double)(*m_transitions)[key] / (double)m_transitions_count;
  }
  inline Emissions GetEmissions(){
    return *m_emissions;
  }
  inline Transitions GetTransitions(){
    return *m_transitions;
  }
};


typedef std::shared_ptr<HmmNode> HmmNodePtr;
typedef std::unordered_map<std::string, HmmNodePtr> HmmNodes;
typedef std::unordered_set<HmmKey> Vocablary;


class Hmm{
 protected:
  std::unique_ptr<HmmNodes> m_nodes;
  std::unique_ptr<Vocablary> m_vocablary;
  inline void AddToVocablary(const HmmKey &key){
    m_vocablary->insert(key);
  };
  inline int GetVocablarySize(){
   return m_vocablary->size();
  }
  inline double GetEmissionProb(const std::string & word, const HmmNodePtr node){
    return node->GetEmissionProb(word, GetVocablarySize());
  }
  inline double GetTransitionProb(const std::string & word, const HmmNodePtr node){
    return node->GetTransitionProb(word);
  }

  inline HmmNodePtr GetNode(const HmmKey & key){
    return (*m_nodes)[key];
  }
  HmmNodePtr CreateNode(const HmmKey &key);
  HmmNodePtr GetOrCreateNode(const HmmKey &);
  void Smoothing();
  
 public:
  Hmm();
  virtual ~Hmm() = default;
 
  void Show(HmmNodePtr node, const std::string &type);
  void Learn(const std::string &filename);
  double Test(const std::string &filename);
  std::vector<HmmKey> Viterbi(const std::vector<std::string>&);


};


#endif
