

#ifndef NGRAM_H_INCLUDED
#define NGRAM_H_INCLUDED


#include <deque>

#include <memory>
#include <vector>
#include <unordered_map>
#include <set>
#include <string>
#include <functional>

#include <map>
#include <unordered_set>

#include "util.h"

class NGram;
class NGramNode;


//================================
//        NGramNode
//================================


typedef unsigned long WordID_t;
typedef std::shared_ptr<NGramNode> NGramNodePtr_t;
typedef std::vector<WordID_t> NGramKey_t;
typedef std::unique_ptr<NGramKey_t> NGramKeyPtr_t;


//unordered_mapで使うvector<string>のハッシュ関数
namespace std {
  template <>
  class hash<NGramKey_t> {
  public:
    size_t operator()(const NGramKey_t& v) const{
      size_t h = 0;
      for(auto it = v.begin(); it < v.end(); it++){
	h = h ^ hash<WordID_t>()(*it);
      }
      return h;
    }
  };
}

typedef std::unordered_map<WordID_t, int> NGramFreq_t;
typedef std::unique_ptr<NGramFreq_t> NGramFreqPtr_t;


class NGramNode{
 private:
  
  NGramKeyPtr_t m_key;
  NGramFreqPtr_t m_freq;
  int m_total;
 public:
  NGramNode(const NGramKey_t &);
  virtual ~NGramNode() = default;
  inline NGramKey_t GetKey() const {return *m_key;};
  inline NGramFreq_t GetFreq() const { return *m_freq;};
  inline int GetFreq(const WordID_t &id) const {
    auto it = m_freq->find(id);
    //キーが設定されている場合はキーバリューのPairが返る
    if(it != m_freq->end()){
      return it->second;
    }else{
      return 0;
    }
  }
  inline int GetTotal() const {return m_total;}
  void AddFreq(const WordID_t &);
  void SetFreq(const WordID_t &, const int);
};


//================================
//          NGram
//================================

typedef StringConverter NGramVocablary_t;
typedef std::unique_ptr<NGramVocablary_t> NGramVocablaryPtr_t;

typedef std::unordered_map<NGramKey_t, NGramNodePtr_t> NGramMap_t;
typedef std::unique_ptr<NGramMap_t> NGramMapPtr_t;



class NGram{
 private:
  virtual inline double GetProb(NGramNodePtr_t node,const WordID_t & id) const{
    if(node->GetTotal() == 0){
      return 0;
    }
    return (double)(node->GetFreq(id)) / (double)(node->GetTotal());
  }
  NGramKey_t StartNodeKey(const int n) const;

 protected:
  
  NGramMapPtr_t m_map;
  NGramVocablaryPtr_t m_vocablary;
  int N;

  NGramNodePtr_t GetOrCreateNode(const NGramKey_t &key);
  NGramNodePtr_t GetNode(const NGramKey_t &key) const;
  inline NGramMap_t GetMap() const {return *m_map;}
  WordID_t AddToVocablary(const std::string&);

  virtual std::string ForwardTransit(NGramNodePtr_t node);
  //virtual WordID_t BackwardTransit(NGramNodePtr_t node);

  double OutputProb(NGramNodePtr_t ,const WordID_t &) const;
  double OutputProb(NGramNodePtr_t) const;
  NGramKey_t StrvToNGramKey(const std::vector<std::string> &) ;
  std::vector<std::string> NGramKeyToStrv(const NGramKey_t &) ;


 public:
  NGram(const int);
  virtual ~NGram() = default;

  void Add(const NGramKey_t &);
  void Learn();
  void Save(const std::string & filename) const;
  void Load(const std::string & filename);
  double SequenceProb(const NGramKey_t &) const;
  double PerplexityTest();
  std::string CreateRandomSentence(const std::string &keywords = "");
}
;


//================================
//    LaplaceSmoothed-NGram
//================================

class LaplaceSmoothedNGram : public NGram{
 private:
  double GetProb(NGramNodePtr_t node,const WordID_t &str) const;

  //std::string ForwardTransit(NGramNodePtr_t node);

 public:
  LaplaceSmoothedNGram(const int n);
  ~LaplaceSmoothedNGram() = default;
};

#endif

