#include "common.h"

//
// split関数
// 第一引数を第二引数の文字で分割したvectorを返す
//
std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    std::string item;
    for (char ch: s)
    {
        if (ch == delim)
        {
            if (!item.empty())
                elems.push_back(item);
            item.clear();
        }
        else
        {
            item += ch;
        }
    }
    if (!item.empty())
        elems.push_back(item);
    return elems;
}

//
// join
// vectorにおけるfirstとlast間の要素をdelimで補完したstringを返す
//
std::string join(vec_iter first, vec_iter last, char delim)
{
    std::stringstream ss;
    for(auto iter = first; iter != last; ++iter)
    {
        ss << *iter;
        if(iter != last-1)
            ss << delim;
    }
    std::string s = ss.str();
    return s;
}

