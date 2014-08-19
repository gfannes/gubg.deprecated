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

    bool almost_equal(long long a, long long b)
    {
        //within 10%
        return (2*10*std::abs(a - b)) <= (std::abs(a) + std::abs(b));
    }
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
        {
            //Make sure we switch back to Void after these loops
            Profiler::RAII raii(profiler, Location::Void);

            for(int i = 0; i < 10; ++i)
            {
                profiler.setLocationTo(Location::A);
                sleep(1);
                profiler.setLocationTo(Location::B);
                sleep(1);
                profiler.setLocationTo(Location::C);
                sleep(2);
            }
        }

        SECTION("relative durations")
        {
            const auto &elapses = profiler.elapses();
            REQUIRE(almost_equal(elapses[1], elapses[2]));
            REQUIRE(almost_equal(2*elapses[1], elapses[3]));
        }
        SECTION("streaming")
        {
            std::ostringstream oss;
            oss << profiler;
            REQUIRE(!oss.str().empty());
            std::cout << oss.str();
        }
    }
}
