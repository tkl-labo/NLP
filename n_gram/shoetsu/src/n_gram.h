

#ifndef NGRAM_H_INCLUDED
#define NGRAM_H_INCLUDED


#include <deque>

#include <memory>
#include <vector>
#include <unordered_map>
#include <set>
#include <string>
#include <functional>


class NGram;
class NGramNode;


//================================
//        NGramNode
//================================


typedef std::shared_ptr<NGramNode> NGramNodePtr_t;
typedef std::vector<std::string> NGramKey_t;
typedef std::unique_ptr<NGramKey_t> NGramKeyPtr_t;


//unordered_mapで使うvector<string>のハッシュ関数
namespace std {
  template <>
  class hash<NGramKey_t> {
  public:
    size_t operator()(const NGramKey_t& v) const{
      size_t h = 0;
      for(auto it = v.begin(); it < v.end(); it++){
	h = h ^ hash<string>()(*it);
      }
      return h;
    }
  };
}

typedef std::unordered_map<std::string, int> NGramFreq_t;
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
  inline int GetFreq(const std::string &str) const {
    auto it = m_freq->find(str);
    //キーが設定されている場合はキーバリューのPairが返る
    if(it != m_freq->end()){
      return it->second;
    }else{
      return 0;
    }
  }
  inline int GetTotal() const {return m_total;}
  void AddFreq(const std::string &);
  void SetFreq(const std::string &, const int);
};


//================================
//          NGram
//================================

typedef std::set<std::string> NGramVocablary_t;
typedef std::unique_ptr<NGramVocablary_t> NGramVocablaryPtr_t;

typedef std::unordered_map<NGramKey_t, NGramNodePtr_t> NGramMap_t;
typedef std::unique_ptr<NGramMap_t> NGramMapPtr_t;



class NGram{
 private:
  virtual inline double GetProb(NGramNodePtr_t node,const std::string &str) const{
    if(node->GetTotal() == 0){
      return 0;
    }
    return (double)(node->GetFreq(str)) / (double)(node->GetTotal());
  }

 protected:
  
  NGramMapPtr_t m_map;
  NGramVocablaryPtr_t m_vocablary;
  int N;

  NGramNodePtr_t GetOrCreateNode(const NGramKey_t &key);
  NGramNodePtr_t GetNode(const NGramKey_t &key) const;
  inline NGramMap_t GetMap() const {return *m_map;}
  inline NGramVocablary_t GetVocablary() const {return *m_vocablary;}
  void AddToVocablary(std::string);

  virtual std::string Transit(NGramNodePtr_t node);
  double OutputProb(NGramNodePtr_t ,const std::string &) const;
  double OutputProb(NGramNodePtr_t) const;
 
 public:
  NGram(const int);
  virtual ~NGram() = default;

  void Add(const NGramKey_t &);
  void Learn();
  void Save(const std::string & filename) const;
  void Load(const std::string & filename);
  double SequenceProb(const NGramKey_t &) const;
  double PerplexityTest();
  std::string CreateRandomSentence();
}
;


//================================
//    LaplaceSmoothed-NGram
//================================

class LaplaceSmoothedNGram : public NGram{
 private:
  inline double GetProb(NGramNodePtr_t node,const std::string &str) const{
    return (double)(node->GetFreq(str) + 1) / (double)(node->GetTotal() + GetVocablary().size() + 1);
  }

  std::string Transit(NGramNodePtr_t node);

 public:
  LaplaceSmoothedNGram(const int n);
  ~LaplaceSmoothedNGram() = default;
};

#endif

