

#ifndef NGRAM_H_INCLUDED
#define NGRAM_H_INCLUDED



#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include <wchar.h>
#include <iostream>

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

typedef std::unordered_map<NGramKey_t, NGramNodePtr_t> NGramMap_t;
typedef std::shared_ptr<NGramMap_t> NGramMapPtr_t;


class NGramNode{
 private:
  NGramKeyPtr_t m_key;
  NGramFreqPtr_t m_freq;
  int m_total;
 public:
  NGramNode(const NGramKey_t &);
  virtual ~NGramNode() = default;
  inline NGramKey_t GetKey(){ return *m_key;};
  inline NGramFreq_t GetFreq(){ return *m_freq;};
  inline int GetFreq(std::string str){
    auto it = m_freq->find(str);
    //キーが設定されている場合はキーバリューのPairが返る
    if(it != m_freq->end()){
      return it->second;
    }else{
      return 0;
    }
  }
  inline float GetProb(std::string str){
    return (float)(*m_freq)[str] / (float)m_total;
  }
  void AddFreq(const std::string &);
  float OutputProb(const std::string &);
  float OutputProb();
 
};



class NGram{
 private:
  NGramMapPtr_t m_map;
  std::string Transit(NGramNodePtr_t node);
  NGramNodePtr_t GetOrCreateNode(NGramKey_t key);
  NGramNodePtr_t GetNode(NGramKey_t key);
  NGramNodePtr_t GetStartNode();

 public:
  NGram(const int n = 2);
  virtual ~NGram() = default;

  int N;
  void Add(NGramKey_t);
  void Learn();

  inline NGramMap_t GetMap(){return *m_map;}


  std::string CreateRandomSentence();
};


#endif

