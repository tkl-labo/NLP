

#ifndef NGRAM_H_INCLUDED
#define NGRAM_H_INCLUDED



#include <memory>
#include <vector>
#include <unordered_map>
#include <set>
#include <string>
#include <functional>


class NGram;
class NGramNode;

typedef std::shared_ptr<NGramNode> NGramNodePtr_t;

typedef std::vector<std::string> NGramKey_t;
typedef std::shared_ptr<NGramKey_t> NGramKeyPtr_t;


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
typedef std::shared_ptr<NGramFreq_t> NGramFreqPtr_t;


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
  inline float GetProb(const std::string &str){
    return (float)(*m_freq)[str] / (float)m_total;
  }
  void AddFreq(const std::string &);
  float OutputProb(const std::string &);
  float OutputProb();
 
};

typedef std::set<std::string> NGramVocablary_t;
typedef std::unique_ptr<NGramVocablary_t> NGramVocablaryPtr_t;

typedef std::unordered_map<NGramKey_t, NGramNodePtr_t> NGramMap_t;
typedef std::shared_ptr<NGramMap_t> NGramMapPtr_t;

class NGram{
 private:
  NGramMapPtr_t m_map;
  NGramVocablaryPtr_t m_vocablary;
  int N;

  std::string Transit(NGramNodePtr_t node);
  NGramNodePtr_t GetOrCreateNode(const NGramKey_t &key);
  NGramNodePtr_t GetNode(const NGramKey_t &key);
  NGramNodePtr_t GetStartNode();
  void AddToVocablary(std::string);
 public:
  NGram(const int n = 2);
  virtual ~NGram() = default;

  void Add(const NGramKey_t &);
  void Learn();

  std::string CreateRandomSentence();
};


#endif

