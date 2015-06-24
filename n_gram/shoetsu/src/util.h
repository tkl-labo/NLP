

#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <string>

double cur_time();

std::wstring widen(const std::string &);
std::string narrow(const std::wstring &src);

#endif
