
#include "util.h"

#include <sstream>
#include <sys/time.h>
#include <wchar.h>
#include <random>

using namespace std;

//=========================
//   Measuring
//=========================

double cur_time(){
  struct timeval tp[1];
  gettimeofday(tp,0);
  return tp->tv_sec + 1.0e-6 * tp->tv_usec;
}


//==========================
//   Vector Operation
//==========================

void normalize(std::vector<double> &vec){
  double sum = 0;
  for(auto it = vec.begin(); it != vec.end(); it++){
    sum += (*it) * (*it);
  }
  sum = sqrt(sum);
  for(auto it = vec.begin(); it != vec.end(); it++){
    *it /= sum;
  }
}


//============================
//   Multi-Byte String
//===========================


wstring widen(const std::string &src) 
{
  wchar_t *wcs = new wchar_t[src.length() + 1];
  mbstowcs(wcs, src.c_str(), src.length() + 1);
  wstring dest = wcs;
  delete [] wcs;
  return dest;
}

string narrow(const std::wstring &src) 
{
  char *mbs = new char[src.length() * MB_CUR_MAX + 1];
  wcstombs(mbs, src.c_str(), src.length() * MB_CUR_MAX + 1);
  string dest = mbs;
  delete [] mbs;
  return dest;
}



//============================
//   Text Parsing
//============================


// vector<string> split(const string &s, const char delim) {
//   vector<string> elems;
//   stringstream ss(s);
//   string item;
//   while (getline(ss, item, delim)) {
//     if (!item.empty()) {
//       elems.push_back(item);
//     }
//   }
//   return elems;
// }


//こっちのほうが速い
vector<string> split(const string &str, const char delim){
  vector<string> res;
  size_t current = 0, found;
  string s;
  while((found = str.find_first_of(delim, current)) != string::npos){
    s = string(str, current, found - current);
    if(!s.empty()){
      res.push_back(s);
    }
    current = found + 1;
  }
  s = string(str, current, str.size() - current);
  if(!s.empty()){
    res.push_back(s);
  }
  return res;
}



//==========================
//       Random
//==========================

namespace Util{

  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution<double> engine(0.0 ,1.0);
  
  double Random(){
    return engine(mt);
  }
};




//============================
//   String and Numeric
//============================


StringConverter::StringConverter(){
  m_str2id = make_unique<unordered_map<string, int>>();
  m_id2str = make_unique<vector<string>>();
  m_count = 0;
}

int StringConverter::str2id(const string& str) const{
  auto it = m_str2id ->find(str);
  if (it != m_str2id ->end()){
    return it->second;
  }else{
    return -1;
  }
}

string StringConverter::id2str(const int id) const{
  string str;
  if(id < GetCount()){
    str = (*m_id2str)[id];
  }else{
    return "";
  }
  return str;
}


int StringConverter::AddStr(const string &str){
  if(m_str2id->find(str) == m_str2id->end()){
    (*m_str2id)[str] = m_count; 
    m_id2str->push_back(str);
    m_count++;
  }
  return m_count-1;
}



