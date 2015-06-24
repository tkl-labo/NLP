

#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <string>
#include <vector>

double cur_time();

std::wstring widen(const std::string &);
std::string narrow(const std::wstring &src);
std::vector<std::string> split(const std::string &s, const char delim = ' ');

#endif
