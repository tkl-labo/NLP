

#ifndef F_HMM_H_INCLUDED
#define F_HMM_H_INCLUDED

#include <functional>
#include "hmm.h"


class F_HmmNode : public HmmNode {
 private:

 public:
  F_HmmNode(const HmmKey key);
  virtual ~F_HmmNode() = default;
  
};


  
class F_Hmm : public Hmm {
 private:
  void SetupClassifiers();
 public:
  F_Hmm();
  virtual ~F_Hmm() = default;
  std::unordered_map<int, std::function<bool(const std::string&)>> Classifiers;
};







#endif
