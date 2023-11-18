#include <iostream>
#include <chrono>
 
int main()
{
    auto now = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(now);
 
    std::cout << "Current Time and Date: " << std::ctime(&end_time) << std::endl;
 
    return 0;
}
