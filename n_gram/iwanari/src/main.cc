#include <chrono>
#include <iostream>

int main(void)
{
    const auto start_time = std::chrono::system_clock::now();
    const auto end_time = std::chrono::system_clock::now();

    const auto time_span = end_time - start_time;

    std::cout << "Elapsed Time: " 
        << std::chrono::duration_cast<std::chrono::milliseconds>(time_span).count() 
        << "[ms]" << std::endl;
    return 0;
}

