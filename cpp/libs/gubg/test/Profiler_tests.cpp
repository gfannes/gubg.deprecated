#include "gubg/Profiler.hpp"
#include <chrono>
#include <thread>
#include <iostream>

namespace  { 
    void sleep(unsigned int ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
    enum class Location
    {
        Void, A, B, C, Nr,
    };
    typedef gubg::Profiler<Location> Profiler;
    Profiler profiler(Location::Void);

    void f()
    {
        profiler.setLocationTo(Location::A);
        sleep(1);
        profiler.setLocationTo(Location::B);
        sleep(1);
        profiler.setLocationTo(Location::C);
        sleep(2);
    }
} 

int main()
{
    for(int i = 0; i < 10; ++i)
        f();
    std::cout << profiler;
    return 0;
}
