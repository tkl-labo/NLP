

#ifndef HMM_H_INCLUDED
#define HMM_H_INCLUDED

#include <memory>
#include <vector>
#include <string.h>
#include <unordered_map>
#include <unordered_set>
#include <functional>

typedef std::string F_Key;
const std::vector<std::string> F_type = {
  "F_CAP",
  "F_CD",
  "F_DT",
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

/* enum F_TYPE{ */
/*   F_CAP, */
/*   F_CD, */
/*   F_DT, */
/*   F_END_ADJ, */
/*   F_END_NOUN, */
/*   F_END_ED, */
/*   F_END_ER, */
/*   F_END_EST, */
/*   F_END_ING, */
/*   F_END_S, */
/*   F_VERB, */
/*   F_OTHERS */
/* }; */


const std::string START_NODE_KEY = "<S>";
const std::vector<std::string> EXCEPTION_KEYS = {",", "."};
const std::string END_LINE = "";


typedef std::string HmmKey;
typedef std::unordered_map<HmmKey, int> Transitions;
typedef std::unordered_map<std::string, int> Emissions;
typedef std::unordered_map<int, int> FeatureEmissions;


#include <iostream>
class HmmNode{
 protected:
  std::unique_ptr<Transitions> m_transitions;
  std::unique_ptr<Emissions> m_emissions;
  std::unique_ptr<FeatureEmissions> m_femissions;
  int m_transitions_count;
  int m_emissions_count;

  const HmmKey m_key;
 public:
  HmmNode(const HmmKey key);
  virtual ~HmmNode() = default;

  HmmKey GetKey() { return m_key;};
  void EmissionLearning(const std::string &);
  void TransitionLearning(const HmmKey &);
  void FeatureEmissionLearning(F_TYPE);

  // 単語の特徴量の記録
  inline double GetFeatureEmissionProb(F_TYPE type, const int n_vocab){
    return (*m_femissions)[type];
    return (double)((*m_femissions)[type]) / (double)(m_emissions_count);
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

  void SetupClassifiers();
  F_TYPE ClassifyUnknown(const std::string &);
  std::unordered_map<int, std::function<bool(const std::string&)>> Classifiers;
  
 public:
  Hmm();
  virtual ~Hmm() = default;
 
  void Show(HmmNodePtr node, const std::string &type);
  void Learn(const std::string &filename);
  double Test(const std::string &filename);
  std::vector<HmmKey> Viterbi(const std::vector<std::string>&);


};


#endif
