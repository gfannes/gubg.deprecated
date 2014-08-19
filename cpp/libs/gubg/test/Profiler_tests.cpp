#include "catch/catch.hpp"
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
} 

TEST_CASE("Profiler", "[profiler]")
{
    Profiler profiler(Location::Void);
    SECTION("ctor has all elapses initialized")
    {
        Profiler::Elapses zeroElapses;
        REQUIRE(zeroElapses == profiler.elapses());
    }
    SECTION("streaming")
    {
        for(int i = 0; i < 10; ++i)
        {
            profiler.setLocationTo(Location::A);
            sleep(1);
            profiler.setLocationTo(Location::B);
            sleep(1);
            profiler.setLocationTo(Location::C);
            sleep(2);
        }
        std::cout << profiler;
    }
}
