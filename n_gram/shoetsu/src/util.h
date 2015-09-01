

#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <string>
#include <vector>
#include <deque>
#include <unordered_map>
#include <memory>

double cur_time();

std::wstring widen(const std::string &);
std::string narrow(const std::wstring &src);


std::vector<std::string> split(const std::string &, const char delim = ' ');

namespace Util{

  double Random();
};



typedef unsigned long StrNum;

class StringConverter{
  private:
  std::unique_ptr<std::unordered_map<std::string, StrNum>> m_str2id ;
  std::unique_ptr<std::vector<std::string>> m_id2str ;
  StrNum  m_count;
  public:
  StringConverter();
  virtual  ~StringConverter() = default;
  StrNum str2id(const std::string &str) const;
  std::string id2str(const StrNum id) const;
  StrNum AddStr(const std::string & str);
  inline StrNum GetCount() const {return m_count;};
};


void normalize(std::vector<double> &);


#endif
