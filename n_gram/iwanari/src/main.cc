#include <iostream>
#include <wchar.h>
#include <memory>
#include "stopwatch.h"
int main(void)
{
    setlocale(LC_ALL, "ja_JP.UTF-8");
    
	util::Stopwatch<std::chrono::milliseconds> sw;

	sw.stop();
	std::cout << "Elapsed: " << sw.showElapsedTime().c_str() << std::endl;
    
    return 0;
}

