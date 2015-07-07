

#ifndef F_HMM_H_INCLUDED
#define F_HMM_H_INCLUDED

#include <functional>
#include "hmm.h"


enum UnknownType{
  F_CAP,
  F_CD,
  F_DT,
  F_END_ADJ,
  F_END_NOUN,
  F_END_ED,
  F_END_ER,
  F_END_EST,
  F_END_ING,
  F_END_S,
  F_VERB,
  F_OTHERS
};

typedef std::unordered_map<int, int> FeatureEmissions;



class F_HmmNode : public HmmNode {
 private:
  std::unique_ptr<FeatureEmissions> m_femissions;
 public:
  F_HmmNode(const HmmKey key);
  virtual ~F_HmmNode() = default;
  //void EmissionLearning(const std::string &);
  void FeatureEmissionLearning(UnknownType);

};

typedef std::shared_ptr<F_HmmNode> F_HmmNodePtr;
typedef std::unordered_map<std::string, F_HmmNodePtr> F_HmmNodes;



class F_Hmm : public Hmm {
 private:
  void SetupClassifiers();
  UnknownType ClassifyUnknown(const std::string &);
  std::unordered_map<int, std::function<bool(const std::string&)>> Classifiers;
 public:
  F_Hmm();
  virtual ~F_Hmm() = default;
  void Learn(const std::string &filename);

};







#endif
