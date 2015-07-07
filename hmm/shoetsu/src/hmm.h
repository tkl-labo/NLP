

#ifndef HMM_H_INCLUDED
#define HMM_H_INCLUDED

#include <memory>
#include <vector>
#include <string.h>
#include <unordered_map>
#include <unordered_set>
#include <functional>

// 未知語のグループ
typedef std::string F_TYPE;
const std::vector<F_TYPE> F_TYPES = {
  "F_CAP",
  "F_CD",
  "F_END_ADJ",
  "F_END_NOUN",
  "F_END_ED",
  "F_END_ER",
  "F_END_EST",
  "F_END_ING",
  "F_END_S",
  "F_VERB",
  "F_OTHERS"
};

const std::string START_NODE_KEY = "<S>";
const std::vector<std::string> EXCEPTION_KEYS = {",", "."};
const std::string END_LINE = "";


typedef std::string HmmKey;
typedef std::unordered_map<HmmKey, int> Transitions;
typedef std::unordered_map<std::string, int> Emissions;
typedef std::unordered_map<F_TYPE, int> FeatureEmissions;


#include <iostream>
class HmmNode{
 protected:
  std::unique_ptr<Transitions> m_transitions;
  std::unique_ptr<Emissions> m_emissions;
  std::unique_ptr<FeatureEmissions> m_femissions;
  int m_transitions_count;
  int m_emissions_count;

  int m_rareword_count;

  const HmmKey m_key;
 public:
  HmmNode(const HmmKey key);
  virtual ~HmmNode() = default;

  HmmKey GetKey() { return m_key;};
  void EmissionLearning(const std::string &);
  void TransitionLearning(const HmmKey &);
  void FeatureEmissionLearning(const F_TYPE, const int);

  // 単語の特徴量の記録
  inline double GetFeatureEmissionProb(F_TYPE type){
    //std::cout << (*m_femissions)[type] << " " << (double)(m_rareword_count) <<std::endl;
    //return (double)((*m_femissions)[type]);
    return (double)((*m_femissions)[type] + 1) / (double)(m_rareword_count + F_TYPES.size());
  }

  //ラプラススムージングを使用
  inline double GetEmissionProb(const std::string &word, const int n_vocab){
    return (double)((*m_emissions)[word] + 1) / (double)(m_emissions_count + n_vocab + 1);
  }
  inline Emissions GetEmissions(){
    return *m_emissions;
  }

  inline double GetTransitionProb(const HmmKey & key){
    return (double)(*m_transitions)[key] / (double)m_transitions_count;
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
  const int RareWordThreshold;
  std::unique_ptr<HmmNodes> m_nodes;
  std::unique_ptr<Vocablary> m_vocablary;
  inline void AddToVocablary(const HmmKey &key){
    m_vocablary->insert(key);
  };
  inline int GetVocablarySize(){
   return m_vocablary->size();
  }

  inline double GetEmissionProb(const std::string & word, const HmmNodePtr node){

    if (m_vocablary->find(word) != m_vocablary->end()){
      return node->GetEmissionProb(word, GetVocablarySize());
    }else{
      //return node->GetEmissionProb(word, GetVocablarySize());
      return node->GetFeatureEmissionProb( ClassifyUnknown(word) );
    }

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

  void SetupClassifiers();
  F_TYPE ClassifyUnknown(const std::string &);
  std::unordered_map<std::string, std::function<bool(const std::string&)>> Classifiers;
  
 public:
  Hmm(const int th);
  virtual ~Hmm() = default;
 
  void Show(HmmNodePtr node, const std::string &type);
  void Learn(const std::string &filename);
  double Test(const std::string &filename);
  std::vector<HmmKey> Viterbi(const std::vector<std::string>&);


};


#endif
