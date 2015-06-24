#include <iostream>

namespace nlp
{
class NGram
{
private:
    std::string m_input;

public:
    NGram(std::string filename) {
        m_input = filename;
    }
    virtual ~NGram() {}
    
    std::string inputFile() { return m_input; }
};
}
