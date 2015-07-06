
#include <memory>
#include <vector>
#include <string>

#include <unordered_map>
#include <unordered_set>


typedef std::string HmmKey;
typedef std::unordered_map<HmmKey, int> Transitions;
typedef std::unordered_map<std::string, int> Emissions;

#include <iostream>
class HmmNode{
 private:
  std::unique_ptr<Transitions> m_transitions;
  std::unique_ptr<Emissions> m_emissions;
  int m_transitions_count;
  int m_emissions_count;

  const HmmKey m_key;
 public:
  HmmNode(const HmmKey key);
  virtual ~HmmNode() = default;
  //inline Transitions GetTransitions(){ return *m_transitions; };
  //inline Emissions GetEmissions(){ return *m_emissions; };
  HmmKey GetKey() { return m_key;};
  void EmissionLearning(const std::string &);
  void TransitionLearning(const HmmKey &);
  inline double GetEmissionProb(const std::string &word){
    return (double)(*m_emissions)[word] / (double)m_emissions_count;
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
 private:
  std::unique_ptr<HmmNodes> m_nodes;
  std::unique_ptr<Vocablary> m_vocablary;
  inline void AddToVocablary(const HmmKey &key){
    m_vocablary->insert(key);
  };
  inline HmmNodePtr GetNode(const HmmKey & key){
    return (*m_nodes)[key];
  }
  HmmNodePtr CreateNode(const HmmKey &key);
  HmmNodePtr GetOrCreateNode(const HmmKey &);
 public:
  Hmm();
  virtual ~Hmm() = default;
 
  void Learn();
  void Show();
  double Test();
  std::vector<HmmKey> Viterbi(const std::vector<std::string>&);
  
  
  //inline std::unordered_set<HmmKey> GetVocablary(){ return *m_vocablary;};
  //inline std::unordered_map<std::string,HmmNode> GetNodes(){return *m_nodes};
};


