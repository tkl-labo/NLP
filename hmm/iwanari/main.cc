#include <iostream>
#include <wchar.h>
#include <memory>
#include "stopwatch.h"

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] 
            << " training_data testing_data [Forward:1 Viterbi:2]" 
            << std::endl;
        return 0;
    }
    
    setlocale(LC_ALL, "ja_JP.UTF-8");
    
    util::Stopwatch<std::chrono::milliseconds> sw;
    sw.stop();
    std::cout << "Elapsed (training): " 
        << sw.showElapsedTime().c_str() << std::endl;
    std::cout << "-------" << std::endl;
    
    sw.start();
    sw.stop();

    std::cout << "Elapsed (test): " 
        << sw.showElapsedTime().c_str() << std::endl;
    std::cout << "-------" << std::endl;
    
    return 0;
}

