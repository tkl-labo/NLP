#pragma once
#include <vector>
#include <string>

namespace nlp
{
class Util
{
public:
    static std::string joinString(const std::vector<std::string> &strings, 
                const std::string &delim);
    static std::string getFirstString(const std::string &str, 
                const std::string &delim);
    static std::vector<std::string> splitString(const std::string &str,
                const std::string &delim);
private:
    Util() {};  // no constructor
};

std::string 
Util::joinString(const std::vector<std::string> &strings, 
    const std::string &delim)
{
    std::string joined_string;
    if (strings.size() > 0) {
        joined_string = strings[0];
        for (int i = 1; i < strings.size(); i++) {
            joined_string += delim;
            joined_string += strings[i];
        }
    }
    return joined_string;
}

std::string 
Util::getFirstString(const std::string &str, 
    const std::string &delim)
{
    size_t fp; // current position, found position
    
    if ((fp =  str.find(delim)) != std::string::npos)
        return str.substr(0, fp);
    
    return str; // EOS has no following delim
}

std::vector<std::string> 
Util::splitString(const std::string &str,
    const std::string &delim)
{
    std::vector<std::string> tokens;

    size_t cp, fp; // current position, found position
    const size_t dsize = delim.size();

    for (cp = 0; 
            (fp = str.find(delim, cp)) != std::string::npos;
            cp = fp + dsize)
        tokens.emplace_back(str, cp, fp - cp);

    tokens.emplace_back(str, cp, str.size() - cp);
    return tokens;
}
    
}
